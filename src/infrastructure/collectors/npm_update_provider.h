#ifndef NPM_UPDATE_PROVIDER_H
#define NPM_UPDATE_PROVIDER_H

#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

namespace my_comp::infrastructure::collectors {

class NpmUpdateProvider : public domain::interfaces::IUpdateProvider {
public:
    explicit NpmUpdateProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "npm"; }

    [[nodiscard]] bool is_available() const override {
        QString output;
        shell_->execute("which", {"npm"}, output);
        return !output.isEmpty();
    }

    [[nodiscard]] std::vector<domain::models::UpdatePackage> check_updates() override {
        QString output;
        std::vector<domain::models::UpdatePackage> updates;

        // npm outdated -g --json retorna exit code 1 quando há updates
        shell_->execute("npm", {"outdated", "-g", "--json"}, output);

        QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
        if (!doc.isObject()) return updates;

        QJsonObject obj = doc.object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            QString name = it.key();
            QJsonObject pkg = it.value().toObject();

            domain::models::UpdatePackage upkg;
            upkg.name = name;
            upkg.current_version = pkg["current"].isString() ? pkg["current"].toString() : "";
            upkg.new_version = pkg["latest"].isString() ? pkg["latest"].toString() : "";
            upkg.source = "npm";
            upkg.provider = domain::models::ProviderType::Npm;
            upkg.description = "Global node package";
            upkg.is_critical = false;
            updates.push_back(upkg);
        }
        return updates;
    }

private:
    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // NPM_UPDATE_PROVIDER_H
