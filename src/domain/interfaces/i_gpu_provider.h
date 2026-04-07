#pragma once

#include "../models/gpu_stats.h"
#include <vector>
#include <memory>

namespace my_comp::domain::interfaces {

using namespace my_comp::domain::models;

class IGpuProvider {
public:
    virtual ~IGpuProvider() = default;
    virtual std::vector<GpuStats> get_all_gpus() = 0;
};

} // namespace my_comp::domain::interfaces
