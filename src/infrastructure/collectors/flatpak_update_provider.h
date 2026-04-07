#ifndef FLATPAK_UPDATE_PROVIDER_H
#define FLATPAK_UPDATE_PROVIDER_H

#include <memory>
#include <QStringList>
#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

using IUpdateProvider = my_comp::domain::interfaces::IUpdateProvider;
using UpdatePackage = my_comp::domain::models::UpdatePackage;

class FlatpakUpdateProvider : public IUpdateProvider {
public:
    explicit FlatpakUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "flatpak"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        return shell_->execute("which", {"flatpak"}, output) == 0;
    }

    [[nodiscard]] std::vector<UpdatePackage> check_updates() override {
        QString output;
        // flatpak remote-ls --updates --columns=name,version,application
        int exit_code = shell_->execute("flatpak", {"remote-ls", "--updates", "--columns=name,version,application"}, output);
        
        std::vector<UpdatePackage> updates;
        if (exit_code == 0) {
            auto lines = output.split("\n", Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                // Flatpak outputs using tabs when columns are requested
                auto parts = line.split("\t", Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    updates.push_back({
                        parts[0].trimmed(), // name
                        "",                 // current
                        parts[1].trimmed(), // new_version
                        "flatpak",          // source
                        parts[2].trimmed(), // app id
                        false
                    });
                }
            }
        }
        return updates;
    }

private:
    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // FLATPAK_UPDATE_PROVIDER_H
