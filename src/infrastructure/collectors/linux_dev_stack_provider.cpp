#include "linux_dev_stack_provider.h"
#include <QStringList>

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
    if (m_executor->execute(cmd, args, output) == 0) {
        QString firstLine = output.split('\n', Qt::SkipEmptyParts).first().trimmed();
        // Limpeza básica para htop style (ex: tirar "Python 3.12.1" -> "3.12.1" ou manter curto)
        return firstLine;
    }
    return "";
}

void LinuxDevStackProvider::collect_toolchains(domain::models::DevStackInfo& info) {
    struct { QString name; QString cmd; QStringList args; } tools[] = {
        {"GCC", "gcc", {"--version"}},
        {"G++", "g++", {"--version"}},
        {"Clang", "clang", {"--version"}},
        {"Python", "python3", {"--version"}},
        {"Go", "go", {"version"}},
        {"Rust", "rustc", {"--version"}},
        {"Node.js", "node", {"--version"}},
        {"Java", "java", {"-version"}} // Java costuma mandar pro stderr, mas o shell executor deve capturar se configurado
    };

    for (const auto& t : tools) {
        QString ver = get_version(t.cmd, t.args);
        if (!ver.isEmpty()) {
            info.toolchains.push_back({t.name, ver, true});
        }
    }
}

void LinuxDevStackProvider::collect_containers(domain::models::DevStackInfo& info) {
    QString output;
    // Formato: ID|Nome|Imagem|Status
    QString format = "{{.ID}}|{{.Names}}|{{.Image}}|{{.Status}}";
    
    // Podman
    if (m_executor->execute("podman", {"ps", "-a", "--format", format}, output) == 0) {
        parse_containers(output, "Podman", info.containers);
    }
    
    // Docker
    if (m_executor->execute("docker", {"ps", "-a", "--format", format}, output) == 0) {
        parse_containers(output, "Docker", info.containers);
    }
}

void LinuxDevStackProvider::parse_containers(const QString& output, const QString& runtime, std::vector<domain::models::ContainerInfo>& containers) {
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QStringList parts = line.split('|');
        if (parts.size() >= 4) {
            containers.push_back({
                parts[0].trimmed(),
                parts[1].trimmed(),
                parts[2].trimmed(),
                parts[3].trimmed(),
                runtime
            });
        }
    }
}

} // namespace my_comp::infrastructure::collectors
