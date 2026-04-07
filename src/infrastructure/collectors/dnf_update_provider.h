#ifndef DNF_UPDATE_PROVIDER_H
#define DNF_UPDATE_PROVIDER_H

#include <memory>
#include <QRegularExpression>
#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

using IUpdateProvider = my_comp::domain::interfaces::IUpdateProvider;
using UpdatePackage = my_comp::domain::models::UpdatePackage;

class DnfUpdateProvider : public IUpdateProvider {
public:
    explicit DnfUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "dnf"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        return shell_->execute("which", {"dnf"}, output) == 0;
    }

    [[nodiscard]] std::vector<UpdatePackage> check_updates() override {
        QString output;
        // dnf check-update returns:
        // 0 if no updates
        // 100 if updates are available
        // 1 if an error occurs
        int exit_code = shell_->execute("dnf", {"check-update", "--quiet"}, output);
        
        std::vector<UpdatePackage> updates;
        if (exit_code == 100) {
            auto lines = output.split("\n", Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                // Ignore lines that don't start with a package name (usually contain .x86_64, .noarch, etc.)
                // A typical line: kernel.x86_64 6.13.0-1.fc43 updates
                auto parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    updates.push_back({
                        parts[0],          // name
                        "",                // current_version (not easily available in check-update without extra calls)
                        parts[1],          // new_version
                        "dnf",             // source
                        parts[2],          // repo/description
                        parts[0].contains("kernel") // critical if it's a kernel update
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

#endif // DNF_UPDATE_PROVIDER_H
