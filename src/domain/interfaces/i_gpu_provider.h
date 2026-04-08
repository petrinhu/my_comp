#pragma once
#include <vector>
#include "../models/gpu_stats.h"

namespace my_comp::domain::interfaces {

class IGpuProvider {
public:
    virtual ~IGpuProvider() = default;

    /**
     * @brief Coleta estatísticas em tempo real de todas as GPUs detectadas.
     */
    virtual std::vector<models::GpuStats> collect_stats() = 0;
};

} // namespace my_comp::domain::interfaces
