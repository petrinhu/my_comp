#include "snap_update_provider.h"
#include <QStringList>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

SnapUpdateProvider::SnapUpdateProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::UpdatePackage> SnapUpdateProvider::check_updates() {
    std::vector<domain::models::UpdatePackage> updates;
    QString output;
    
    // snap refresh --list output format (approx):
    // Name    Version  Rev  Publisher  Notes
    // core22  2024...  1122 canonical* -
    if (m_executor->execute("snap", {"refresh", "--list"}, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        // Pula o cabeçalho se existir
        for (int i = 1; i < lines.size(); ++i) {
            QStringList parts = lines[i].trimmed().split(QRegularExpression("\\s+"));
            if (parts.size() >= 2) {
                domain::models::UpdatePackage pkg;
                pkg.name = parts[0];
                pkg.new_version = parts[1];
                pkg.source = "snap";
                pkg.provider = domain::models::ProviderType::Snap;
                updates.push_back(pkg);
            }
        }
    }
    return updates;
}

bool SnapUpdateProvider::is_available() const {
    QString output;
    return m_executor->execute("snap", {"--version"}, output) == 0;
}

} // namespace my_comp::infrastructure::collectors
