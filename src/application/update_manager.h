#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include <vector>
#include <memory>
#include <algorithm>
#include "../domain/interfaces/i_update_provider.h"
#include "../domain/interfaces/i_installer.h"
#include "../domain/models/update_package.h"

namespace my_comp::application {

using IUpdateProvider = my_comp::domain::interfaces::IUpdateProvider;
using IInstaller = my_comp::domain::interfaces::IInstaller;
using UpdatePackage = my_comp::domain::models::UpdatePackage;

class UpdateManager {
public:
    virtual ~UpdateManager() = default;

    void set_installer(std::shared_ptr<IInstaller> installer) {
        m_installer = std::move(installer);
    }

    virtual void upgrade_all() {
        if (m_installer) {
            m_installer->upgradeAll();
        }
    }

    virtual void register_provider(std::unique_ptr<IUpdateProvider> provider) {
        m_providers.push_back(std::move(provider));
    }

    [[nodiscard]] virtual std::vector<UpdatePackage> check_all_updates() {
        std::vector<UpdatePackage> all_updates;
        for (const auto& provider : m_providers) {
            if (provider && provider->is_available()) {
                auto updates = provider->check_updates();
                all_updates.insert(all_updates.end(), 
                                 std::make_move_iterator(updates.begin()), 
                                 std::make_move_iterator(updates.end()));
            }
        }
        return all_updates;
    }

private:
    std::shared_ptr<IInstaller> m_installer;
    std::vector<std::unique_ptr<IUpdateProvider>> m_providers;
};

} // namespace my_comp::application

#endif // UPDATE_MANAGER_H
