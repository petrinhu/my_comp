#ifndef NPM_UPDATE_PROVIDER_H
#define NPM_UPDATE_PROVIDER_H

#include <memory>
#include <QJsonDocument>
#include <QJsonObject>
#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

using IUpdateProvider = my_comp::domain::interfaces::IUpdateProvider;
using UpdatePackage = my_comp::domain::models::UpdatePackage;

class NpmUpdateProvider : public IUpdateProvider {
public:
    explicit NpmUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "npm"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        return shell_->execute("which", {"npm"}, output) == 0;
    }

    [[nodiscard]] std::vector<UpdatePackage> check_updates() override {
        QString output;
        // npm outdated -g --json returns exit code 1 if updates found
        shell_->execute("npm", {"outdated", "-g", "--json"}, output);
        
        std::vector<UpdatePackage> updates;
        QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject root = doc.object();
            for (auto it = root.begin(); it != root.end(); ++it) {
                QString name = it.key();
                QJsonObject pkg = it.value().toObject();
                updates.push_back({
                    name,
                    pkg["current"].isString() ? pkg["current"].toString() : "",
                    pkg["latest"].isString() ? pkg["latest"].toString() : "",
                    "npm",
                    "Global node package",
                    false
                });
            }
        }
        return updates;
    }

private:
    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // NPM_UPDATE_PROVIDER_H
