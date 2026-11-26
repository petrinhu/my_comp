#include "apt_update_provider.h"
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

AptUpdateProvider::AptUpdateProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::UpdatePackage> AptUpdateProvider::check_updates() {
    std::vector<domain::models::UpdatePackage> updates;
    QString output;
    
    // apt list --upgradable output:
    // package/distro version architecture [upgradable from: old_version]
    if (m_executor->execute("apt", {"list", "--upgradable"}, output) == 0) {
        const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            if (line.contains("Listing...")) continue;
            
            // Regex para capturar nome e versão
            // Ex: "bash/focal-updates 5.0-6ubuntu1.2 amd64 [upgradable from: 5.0-6ubuntu1.1]"
            const QRegularExpression re("^([^/]+)/\\S+\\s+(\\S+)\\s+\\S+\\s+\\[upgradable from:\\s+(\\S+)\\]");
            auto match = re.match(line.trimmed());
            if (match.hasMatch()) {
                domain::models::UpdatePackage pkg;
                pkg.name = match.captured(1);
                pkg.new_version = match.captured(2);
                pkg.current_version = match.captured(3);
                pkg.source = "apt";
                pkg.provider = domain::models::ProviderType::Apt;
                updates.push_back(pkg);
            }
        }
    }
    return updates;
}

bool AptUpdateProvider::is_available() const {
    QString output;
    return m_executor->execute("apt", {"--version"}, output) == 0;
}

} // namespace my_comp::infrastructure::collectors
