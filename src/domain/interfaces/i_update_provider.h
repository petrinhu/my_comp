#ifndef I_UPDATE_PROVIDER_H
#define I_UPDATE_PROVIDER_H

#include <vector>
#include <QString>
#include "../models/update_package.h"

namespace my_comp::domain::interfaces {

using UpdatePackage = my_comp::domain::models::UpdatePackage;

class IUpdateProvider {
public:
    virtual ~IUpdateProvider() = default;

    [[nodiscard]] virtual QString get_name() const = 0;
    [[nodiscard]] virtual bool is_available() const = 0;
    [[nodiscard]] virtual std::vector<UpdatePackage> check_updates() = 0;
};

} // namespace my_comp::domain::interfaces

#endif // I_UPDATE_PROVIDER_H
