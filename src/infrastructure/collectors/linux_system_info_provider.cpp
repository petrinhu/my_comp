#include "linux_system_info_provider.h"
#include <QFile>
#include <QTextStream>

namespace my_comp::infrastructure::collectors {

LinuxSystemInfoProvider::LinuxSystemInfoProvider(std::shared_ptr<IShellExecutor> shell)
    : m_shell(std::move(shell)) {}

domain::models::SystemInfo LinuxSystemInfoProvider::get_system_info() {
    domain::models::SystemInfo info;
    
    // Distro Info
    QFile os_release("/etc/os-release");
    if (os_release.open(QIODevice::ReadOnly)) {
        QTextStream in(&os_release);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            if (line.startsWith("NAME=")) info.distroName = line.mid(5).replace("\"", "");
            if (line.startsWith("VERSION_ID=")) info.distroVersion = line.mid(11).replace("\"", "");
        }
    }

    // Kernel Info
    QString output;
    m_shell->execute("uname", {"-r"}, output);
    info.kernelVersion = output.trimmed();

    // Driver Info (Simples)
    m_shell->execute("lspci", {"-k"}, output);
    if (output.contains("nvidia")) info.gpuDriver = "NVIDIA";
    else if (output.contains("amdgpu")) info.gpuDriver = "AMD";
    else info.gpuDriver = "Generic/Mesa";

    info.kernelUpdateAvailable = false; // Placeholder lógico
    return info;
}

} // namespace my_comp::infrastructure::collectors
