#pragma once
#include <QString>
#include "../models/update_package.h"

namespace my_comp::domain::interfaces {

class IInstaller {
public:
    virtual ~IInstaller() = default;
    virtual void install(const models::UpdatePackage& package) = 0;
};

} // namespace my_comp::domain::interfaces
