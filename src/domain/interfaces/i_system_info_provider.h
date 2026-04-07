#pragma once
#include "../models/system_info.h"

namespace my_comp::domain::interfaces {

class ISystemInfoProvider {
public:
    virtual ~ISystemInfoProvider() = default;
    virtual models::SystemInfo get_system_info() = 0;
};

} // namespace my_comp::domain::interfaces
