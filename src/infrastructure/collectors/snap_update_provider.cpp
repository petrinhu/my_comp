#include "snap_update_provider.h"
#include <QStringList>
#include <QRegularExpression>
#include <QHash>

namespace my_comp::infrastructure::collectors {

SnapUpdateProvider::SnapUpdateProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::UpdatePackage> SnapUpdateProvider::check_updates() {
    std::vector<domain::models::UpdatePackage> updates;
    QString output;

    if (m_executor->execute("snap", {"refresh", "--list"}, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
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
        // IN-07a: enriquecer current_version via `snap list` (uma chamada).
        QString list_out;
        if (m_executor->execute("snap", {"list"}, list_out) == 0) {
            QHash<QString, QString> installed;
            const QStringList list_lines = list_out.split('\n', Qt::SkipEmptyParts);
            for (int i = 1; i < list_lines.size(); ++i) {
                const QStringList sp = list_lines[i].trimmed().split(QRegularExpression("\\s+"));
                if (sp.size() >= 2) installed.insert(sp[0], sp[1]);
            }
            for (auto& u : updates) {
                if (installed.contains(u.name)) u.current_version = installed.value(u.name);
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
