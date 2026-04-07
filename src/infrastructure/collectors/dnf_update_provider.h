#ifndef DNF_UPDATE_PROVIDER_H
#define DNF_UPDATE_PROVIDER_H

#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <QRegularExpression>
#include <QStringList>
#include <memory>

namespace my_comp::infrastructure::collectors {

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class DnfUpdateProvider : public IUpdateProvider {
public:
    explicit DnfUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "dnf"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        shell_->execute("which", {"dnf"}, output);
        return !output.isEmpty();
    }

    [[nodiscard]] std::vector<UpdatePackage> check_updates() override {
        QString output;
        std::vector<UpdatePackage> updates;
        
        if (shell_->execute("dnf", {"check-update", "--quiet"}, output) == 100) {
            auto lines = output.split('\n', Qt::SkipEmptyParts);
            for (const auto& line : lines) {
                auto parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 3) {
                    UpdatePackage pkg;
                    pkg.name = parts[0];
                    pkg.current_version = "";
                    pkg.new_version = parts[1];
                    pkg.source = "dnf";
                    pkg.provider = ProviderType::Dnf;
                    pkg.description = parts[2];
                    pkg.is_critical = parts[0].contains("kernel");
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

#endif // DNF_UPDATE_PROVIDER_H
