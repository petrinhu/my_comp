#ifndef FLATPAK_UPDATE_PROVIDER_H
#define FLATPAK_UPDATE_PROVIDER_H

#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <QStringList>
#include <memory>

namespace my_comp::infrastructure::collectors {

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class FlatpakUpdateProvider : public IUpdateProvider {
public:
    explicit FlatpakUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "flatpak"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        shell_->execute("which", {"flatpak"}, output);
        return !output.isEmpty();
    }

    [[nodiscard]] std::vector<UpdatePackage> check_updates() override {
        QString output;
        std::vector<UpdatePackage> updates;
        
        if (shell_->execute("flatpak", {"remote-ls", "--updates", "--columns=name,version,application"}, output) == 0) {
            auto lines = output.split('\n', Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                auto parts = line.split('\t', Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    UpdatePackage pkg;
                    pkg.name = parts[0].trimmed();
                    pkg.current_version = "";
                    pkg.new_version = parts[1].trimmed();
                    pkg.source = "flatpak";
                    pkg.provider = ProviderType::Flatpak;
                    pkg.description = parts[2].trimmed();
                    pkg.is_critical = false;
                    updates.push_back(pkg);
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
