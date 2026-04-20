#include "linux_dev_stack_provider.h"
#include <QStringList>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureSynchronizer>

namespace my_comp::infrastructure::collectors {

LinuxDevStackProvider::LinuxDevStackProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

domain::models::DevStackInfo LinuxDevStackProvider::collect_dev_stacks() {
    domain::models::DevStackInfo info;
    collect_toolchains(info);
    collect_containers(info);
    return info;
}

QString LinuxDevStackProvider::get_version(const QString& cmd, const QStringList& args) {
    QString output;
    if (m_executor->execute(cmd, args, output) != 0) return {};

    const QString firstLine = output.split('\n', Qt::SkipEmptyParts).value(0).trimmed();
    // Extrai "X.Y" ou "X.Y.Z" de strings tipo "gcc (GCC) 14.0.1" / "go version go1.22.1 linux/amd64".
    static const QRegularExpression kVersionRe(QStringLiteral("\\d+\\.\\d+(?:\\.\\d+)?"));
    const auto match = kVersionRe.match(firstLine);
    return match.hasMatch() ? match.captured(0) : firstLine;
}

void LinuxDevStackProvider::collect_toolchains(domain::models::DevStackInfo& info) {
    struct Tool { QString name; QString cmd; QStringList args; };
    const Tool tools[] = {
        {"GCC",     "gcc",     {"--version"}},
        {"G++",     "g++",     {"--version"}},
        {"Clang",   "clang",   {"--version"}},
        {"Python",  "python3", {"--version"}},
        {"Go",      "go",      {"version"}},
        {"Rust",    "rustc",   {"--version"}},
        {"Node.js", "node",    {"--version"}},
        // Java escreve em stderr; usamos sh -c "... 2>&1" para redirecionar sem alterar IShellExecutor.
        {"Java",    "sh",      {"-c", "java -version 2>&1"}}
    };

    // Paralelizado: cada toolchain executa em uma thread do pool. Ordem final preservada
    // pela iteração sequencial sobre o array (i) após o wait-all.
    constexpr std::size_t kCount = sizeof(tools) / sizeof(tools[0]);
    QFuture<QString> futures[kCount];
    QFutureSynchronizer<QString> sync;
    for (std::size_t i = 0; i < kCount; ++i) {
        futures[i] = QtConcurrent::run([this, t = tools[i]]() {
            return get_version(t.cmd, t.args);
        });
        sync.addFuture(futures[i]);
    }
    sync.waitForFinished();

    for (std::size_t i = 0; i < kCount; ++i) {
        const QString ver = futures[i].result();
        if (!ver.isEmpty()) {
            info.toolchains.push_back({tools[i].name, ver});
        }
    }
}

void LinuxDevStackProvider::collect_containers(domain::models::DevStackInfo& info) {
    QString output;

    // Podman: `ps --format json` retorna array JSON com todos os containers.
    if (m_executor->execute("podman", {"ps", "-a", "--format", "json"}, output) == 0) {
        parse_containers_json(output, "Podman", info.containers);
    }

    // Docker: `ps --format json` retorna um objeto JSON por linha (NDJSON).
    if (m_executor->execute("docker", {"ps", "-a", "--format", "json"}, output) == 0) {
        parse_containers_json(output, "Docker", info.containers);
    }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) — output é payload JSON, runtime é "Podman"/"Docker"; call sites literais dificultam o swap acidental.
void LinuxDevStackProvider::parse_containers_json(const QString& output, const QString& runtime, std::vector<domain::models::ContainerInfo>& containers) {
    const QString trimmed = output.trimmed();
    if (trimmed.isEmpty()) return;

    // Caso 1: array JSON completo (padrão do podman).
    if (trimmed.startsWith('[')) {
        const auto doc = QJsonDocument::fromJson(trimmed.toUtf8());
        if (!doc.isArray()) return;
        for (const auto& v : doc.array()) {
            if (v.isObject()) parse_container_object(v.toObject(), runtime, containers);
        }
        return;
    }

    // Caso 2: NDJSON — um objeto por linha (padrão do docker).
    const QStringList lines = trimmed.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        const auto doc = QJsonDocument::fromJson(line.toUtf8());
        if (doc.isObject()) parse_container_object(doc.object(), runtime, containers);
    }
}

void LinuxDevStackProvider::parse_container_object(const QJsonObject& obj, const QString& runtime, std::vector<domain::models::ContainerInfo>& containers) {
    // Podman usa "Id"; docker usa "ID". Nomes: podman → array "Names", docker → string "Names".
    const QString id = obj.value("Id").toString(obj.value("ID").toString());

    QString name;
    const auto namesVal = obj.value("Names");
    if (namesVal.isArray()) {
        const auto arr = namesVal.toArray();
        if (!arr.isEmpty()) name = arr.first().toString();
    } else {
        name = namesVal.toString();
    }

    const QString image  = obj.value("Image").toString();
    const QString status = obj.value("Status").toString(obj.value("State").toString());

    if (id.isEmpty() && name.isEmpty()) return;

    containers.push_back({
        id.trimmed(),
        name.trimmed(),
        image.trimmed(),
        status.trimmed(),
        runtime
    });
}

} // namespace my_comp::infrastructure::collectors
