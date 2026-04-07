#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include <vector>
#include <memory>
#include <algorithm>
#include "interfaces/i_update_provider.h"
#include "models/update_package.h"

namespace my_comp::domain {

using IUpdateProvider = my_comp::domain::interfaces::IUpdateProvider;
using UpdatePackage = my_comp::domain::models::UpdatePackage;

class UpdateManager {
public:
    void register_provider(std::unique_ptr<IUpdateProvider> provider) {
        providers_.push_back(std::move(provider));
    }

    [[nodiscard]] std::vector<UpdatePackage> check_all_updates() {
        std::vector<UpdatePackage> all_updates;
        for (const auto& provider : providers_) {
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
    std::vector<std::unique_ptr<IUpdateProvider>> providers_;
};

} // namespace my_comp::domain

#endif // UPDATE_MANAGER_H
