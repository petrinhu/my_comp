#pragma once
#include "../models/hardware_topology.h"

namespace my_comp::domain::interfaces {

class IHardwareProvider {
public:
    virtual ~IHardwareProvider() = default;
    virtual models::HardwareTopology collect_topology() = 0;
};

} // namespace my_comp::domain::interfaces
