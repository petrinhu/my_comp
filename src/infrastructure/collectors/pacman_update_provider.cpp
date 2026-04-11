#include "pacman_update_provider.h"
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

PacmanUpdateProvider::PacmanUpdateProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::UpdatePackage> PacmanUpdateProvider::check_updates() {
    std::vector<domain::models::UpdatePackage> updates;
    QString output;
    
    // pacman -Qu output:
    // package_name old_version -> new_version
    if (m_executor->execute("pacman", {"-Qu"}, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList parts = line.trimmed().split(QRegularExpression("\\s+"));
            if (parts.size() >= 4 && parts[2] == "->") {
                domain::models::UpdatePackage pkg;
                pkg.name = parts[0];
                pkg.current_version = parts[1];
                pkg.new_version = parts[3];
                pkg.source = "pacman";
                pkg.provider = domain::models::ProviderType::Pacman;
                updates.push_back(pkg);
            }
        }
    }
    return updates;
}

bool PacmanUpdateProvider::is_available() const {
    QString output;
    return m_executor->execute("pacman", {"--version"}, output) == 0;
}

} // namespace my_comp::infrastructure::collectors
