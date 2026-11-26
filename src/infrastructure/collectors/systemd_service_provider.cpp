#include "systemd_service_provider.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringList>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace my_comp::infrastructure::collectors {

SystemdServiceProvider::SystemdServiceProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::get_services() {
    auto system_units = collect_units(domain::models::ServiceScope::System);
    auto user_units = collect_units(domain::models::ServiceScope::User);

    std::vector<domain::models::ServiceInfo> all;
    all.reserve(system_units.size() + user_units.size());
    all.insert(all.end(), system_units.begin(), system_units.end());
    all.insert(all.end(), user_units.begin(), user_units.end());
    return all;
}

namespace {
// Instância de template ("getty@tty1.service") herda o estado do template ("getty@.service").
QString template_of(const QString& unit) {
    const auto at = unit.indexOf(QLatin1Char('@'));
    const auto dot = unit.lastIndexOf(QLatin1Char('.'));
    if (at < 0 || dot < at) return {};
    return unit.left(at + 1) + unit.mid(dot);
}
} // namespace

QHash<QString, QString> SystemdServiceProvider::collect_unit_file_states(domain::models::ServiceScope scope) {
    QHash<QString, QString> states;
    QStringList args = {"list-unit-files", "--type=service", "--no-pager", "--output=json"};
    if (scope == domain::models::ServiceScope::User) args.prepend("--user");
    QString output;
    if (m_executor->execute("systemctl", args, output) != 0) return states;
    const QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (!doc.isArray()) return states;
    for (const QJsonValue& v : doc.array()) {
        const QJsonObject o = v.toObject();
        const QString file = o.value("unit_file").toString();
        if (!file.isEmpty()) states.insert(file, o.value("state").toString());
    }
    return states;
}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::collect_units(domain::models::ServiceScope scope) {
    std::vector<domain::models::ServiceInfo> units;
    const bool user_mode = (scope == domain::models::ServiceScope::User);
    QString output;
    // SV-01c: JSON (systemd >= 246) em vez de colunas separadas por espaço, que
    // truncavam descrições e quebravam com mudança de layout.
    QStringList args = {"list-units", "--type=service", "--all", "--no-pager", "--output=json"};
    if (user_mode) args.prepend("--user");

    if (m_executor->execute("systemctl", args, output) != 0) return units;

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        qWarning() << "[Systemd] list-units não devolveu JSON válido:" << err.errorString();
        return units;
    }
    // SV-01a: estado de habilitação real, uma chamada em lote por escopo.
    const auto file_states = collect_unit_file_states(scope);
    for (const QJsonValue& v : doc.array()) {
        const QJsonObject o = v.toObject();
        const QString unit = o.value("unit").toString();
        if (unit.isEmpty()) continue;
        domain::models::ServiceInfo info;
        info.name = unit;
        info.loadState = o.value("load").toString();
        info.activeState = o.value("active").toString();
        info.subState = o.value("sub").toString();
        info.description = o.value("description").toString();
        info.scope = scope;
        // "enabled"/"enabled-runtime" sobem no boot; static/disabled/masked/indirect não.
        const QString state = file_states.value(unit, file_states.value(template_of(unit)));
        info.isEnabled = state.startsWith(QLatin1String("enabled"));
        units.push_back(info);
    }
    return units;
}

bool SystemdServiceProvider::run_systemctl(const QStringList& args, bool user_mode) {
    QString output;
    QStringList finalArgs = args;
    if (user_mode) finalArgs.prepend("--user");
    return m_executor->execute("systemctl", finalArgs, output) == 0;
}

domain::models::OperationResult SystemdServiceProvider::control(const QStringList& args,
                                                                domain::models::ServiceScope scope,
                                                                const QString& done, const QString& failed) {
    QStringList final_args = args;
    if (scope == domain::models::ServiceScope::User) final_args.prepend("--user");
    const auto r = m_executor->run_capturing("systemctl", final_args);
    if (r.exit_code == 0) return {.ok = true, .message = done};
    // SV-02c: o motivo real (ex.: "Access denied") chega à UI; só a 1ª linha cabe no toast.
    const QString reason = r.err.section(QLatin1Char('\n'), 0, 0).trimmed();
    return {.ok = false, .message = reason.isEmpty() ? failed : failed + ": " + reason};
}

domain::models::OperationResult SystemdServiceProvider::enable_and_start(const QString& name, domain::models::ServiceScope scope) {
    return control({"enable", "--now", name}, scope,
                   QStringLiteral("%1 habilitado no boot e iniciado.").arg(name),
                   QStringLiteral("Falha ao habilitar %1").arg(name));
}

domain::models::OperationResult SystemdServiceProvider::stop(const QString& name, domain::models::ServiceScope scope) {
    return control({"stop", name}, scope,
                   QStringLiteral("%1 parado nesta sessão (continua habilitado no boot, se estava).").arg(name),
                   QStringLiteral("Falha ao parar %1").arg(name));
}

domain::models::OperationResult SystemdServiceProvider::disable_and_stop(const QString& name, domain::models::ServiceScope scope) {
    return control({"disable", "--now", name}, scope,
                   QStringLiteral("%1 desabilitado no boot e parado.").arg(name),
                   QStringLiteral("Falha ao desabilitar %1").arg(name));
}

namespace {

QString unit_dir_path() {
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
}

// Linha ExecStart de um unit já gravado (vazia se o arquivo não existe/não tem).
QString exec_start_of(const QString& unit_path) {
    QFile f(unit_path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    const QString prefix = QStringLiteral("ExecStart=");
    while (!f.atEnd()) {
        const QString line = QString::fromUtf8(f.readLine()).trimmed();
        if (line.startsWith(prefix)) return line.mid(prefix.size());
    }
    return {};
}

bool unit_belongs_to(const QString& unit_path, const QString& exe_path) {
    const QString exec = exec_start_of(unit_path);
    return exec == exe_path || exec.startsWith(exe_path + QLatin1Char(' '));
}

// SV-03f: <base>-daemon.service; se já existe um unit desse nome para OUTRO executável,
// acrescenta um hash curto do caminho para não sobrescrevê-lo.
QString choose_unit_name(const QFileInfo& exe) {
    const QString base = exe.baseName().toLower();
    const QString simple = base + "-daemon.service";
    const QString simple_path = unit_dir_path() + "/" + simple;
    if (!QFile::exists(simple_path) || unit_belongs_to(simple_path, exe.absoluteFilePath())) {
        return simple;
    }
    const QString hash = QString::fromLatin1(
        QCryptographicHash::hash(exe.absoluteFilePath().toUtf8(), QCryptographicHash::Sha256).toHex().left(8));
    return base + "-" + hash + "-daemon.service";
}

} // namespace

domain::models::OperationResult SystemdServiceProvider::create_daemon(const QString& exe_path, const QString& params) {
    auto has_control_char = [](const QString& s) {
        for (QChar c : s) {
            if (c == QChar('\n') || c == QChar('\r') || c == QChar('\0')) return true;
        }
        return false;
    };
    if (has_control_char(exe_path) || has_control_char(params)) {
        return {.ok = false, .message = QStringLiteral("Caminho ou parâmetros contêm quebra de linha: recusado.")};
    }

    // SV-03c: unit que aponta para algo que não roda nasce quebrado; recusar antes de gravar.
    const QFileInfo exeInfo(exe_path);
    if (exeInfo.isRelative()) {
        return {.ok = false, .message = QStringLiteral("Informe o caminho absoluto do executável (ex.: /usr/bin/%1).").arg(exe_path)};
    }
    if (!exeInfo.exists()) {
        return {.ok = false, .message = QStringLiteral("O arquivo %1 não existe.").arg(exe_path)};
    }
    if (!exeInfo.isFile() || !exeInfo.isExecutable()) {
        return {.ok = false, .message = QStringLiteral("%1 não é um arquivo executável (falta permissão de execução?).").arg(exe_path)};
    }

    const QString serviceName = choose_unit_name(exeInfo);
    const QString configDir = unit_dir_path();
    QDir().mkpath(configDir);

    QFile file(configDir + "/" + serviceName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return {.ok = false, .message = QStringLiteral("Sem permissão para gravar %1.").arg(file.fileName())};
    }

    QTextStream out(&file);
    out << "[Unit]\n";
    out << "Description=Daemon for " << exeInfo.fileName() << " (created by my_comp)\n";
    out << "After=network.target\n\n";
    out << "[Service]\n";
    out << "ExecStart=" << exeInfo.absoluteFilePath() << (params.isEmpty() ? QString() : " " + params) << "\n";
    out << "Restart=always\n";
    out << "Type=simple\n\n";
    out << "[Install]\n";
    out << "WantedBy=default.target\n";
    file.close();

    // Reload e Iniciar
    run_systemctl({"daemon-reload"}, true);
    if (!run_systemctl({"enable", "--now", serviceName}, true)) {
        return {.ok = false, .message = QStringLiteral("Unit %1 gravado, mas o systemctl não conseguiu iniciá-lo.").arg(serviceName)};
    }
    return {.ok = true, .message = QStringLiteral("Daemon %1 criado e iniciado.").arg(serviceName)};
}

} // namespace my_comp::infrastructure::collectors
