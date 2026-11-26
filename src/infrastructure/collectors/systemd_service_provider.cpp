#include "systemd_service_provider.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

SystemdServiceProvider::SystemdServiceProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::get_services() {
    auto system_units = collect_units(false);
    auto user_units = collect_units(true);
    
    std::vector<domain::models::ServiceInfo> all;
    all.reserve(system_units.size() + user_units.size());
    all.insert(all.end(), system_units.begin(), system_units.end());
    all.insert(all.end(), user_units.begin(), user_units.end());
    return all;
}

std::vector<domain::models::ServiceInfo> SystemdServiceProvider::collect_units(bool user_mode) {
    std::vector<domain::models::ServiceInfo> units;
    QString output;
    // O comando list-units mostra o estado ativo
    QStringList args = {"list-units", "--type=service", "--all", "--no-legend", "--no-pager"};
    if (user_mode) args.prepend("--user");
    
    if (m_executor->execute("systemctl", args, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList parts = line.trimmed().split(QRegularExpression("\\s+"));
            if (parts.size() >= 5) {
                domain::models::ServiceInfo info;
                info.name = parts[0];
                if (user_mode) info.name += " (User)";
                info.loadState = parts[1];
                info.activeState = parts[2];
                info.subState = parts[3];
                info.description = parts.mid(4).join(' ');
                
                // Otimização: Não chamar is-enabled para cada unit no loop inicial.
                // Considerar 'enabled' se activeState for active por enquanto, 
                // ou marcar como pendente para auditoria futura se necessário.
                info.isEnabled = (info.loadState == "loaded"); 
                
                units.push_back(info);
            }
        }
    }
    return units;
}

bool SystemdServiceProvider::run_systemctl(const QStringList& args, bool user_mode) {
    QString output;
    QStringList finalArgs = args;
    if (user_mode) finalArgs.prepend("--user");
    return m_executor->execute("systemctl", finalArgs, output) == 0;
}

bool SystemdServiceProvider::start_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"enable", "--now", clean_name}, is_user);
}

bool SystemdServiceProvider::pause_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"stop", clean_name}, is_user);
}

bool SystemdServiceProvider::stop_service(const QString& name) {
    bool is_user = name.endsWith(" (User)");
    QString clean_name = name;
    if (is_user) clean_name.remove(" (User)");
    return run_systemctl({"disable", "--now", clean_name}, is_user);
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
