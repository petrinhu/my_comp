#pragma once

#include "../models/power_stats.h"
#include <vector>

namespace my_comp::domain::interfaces {

using namespace my_comp::domain::models;

class IPowerProvider {
public:
    virtual ~IPowerProvider() = default;
    virtual std::vector<PowerStats> get_power_data() = 0;
};

} // namespace my_comp::domain::interfaces
