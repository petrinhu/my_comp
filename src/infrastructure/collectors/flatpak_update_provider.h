#ifndef FLATPAK_UPDATE_PROVIDER_H
#define FLATPAK_UPDATE_PROVIDER_H

#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <QStringList>
#include <QHash>
#include <memory>

namespace my_comp::infrastructure::collectors {

class FlatpakUpdateProvider : public domain::interfaces::IUpdateProvider {
public:
    explicit FlatpakUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "flatpak"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        shell_->execute("which", {"flatpak"}, output);
        return !output.isEmpty();
    }

    [[nodiscard]] std::vector<domain::models::UpdatePackage> check_updates() override {
        QString output;
        std::vector<domain::models::UpdatePackage> updates;

        if (shell_->execute_with_timeout("flatpak", {"remote-ls", "--updates", "--columns=name,version,application"}, output, PACKAGE_MANAGER_TIMEOUT_MS) == 0) {
            auto lines = output.split('\n', Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                auto parts = line.split('\t', Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    domain::models::UpdatePackage pkg;
                    pkg.name = parts[0].trimmed();
                    pkg.new_version = parts[1].trimmed();
                    pkg.source = "flatpak";
                    pkg.provider = domain::models::ProviderType::Flatpak;
                    const QString app_id = parts[2].trimmed();
                    pkg.source_repo = app_id;
                    pkg.description = app_id; // compat
                    pkg.is_critical = false;
                    updates.push_back(pkg);
                }
            }
            enrich_current_versions(updates);
        }
        return updates;
    }

private:
    void enrich_current_versions(std::vector<domain::models::UpdatePackage>& updates) const {
        if (updates.empty()) return;
        QString out;
        if (shell_->execute("flatpak", {"list", "--app", "--columns=application,version"}, out) != 0) return;
        QHash<QString, QString> installed;
        for (const auto& line : out.split('\n', Qt::SkipEmptyParts)) {
            const auto sp = line.split('\t', Qt::SkipEmptyParts);
            if (sp.size() >= 2) installed.insert(sp[0].trimmed(), sp[1].trimmed());
        }
        for (auto& u : updates) {
            if (installed.contains(u.source_repo)) u.current_version = installed.value(u.source_repo);
        }
    }

    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // FLATPAK_UPDATE_PROVIDER_H
