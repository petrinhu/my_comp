#pragma once
#include <memory>
#include "../models/cpu_stats.h"
#include "../models/cpu_architecture.h"

namespace my_comp::domain::interfaces {

class ICpuProvider {
public:
    virtual ~ICpuProvider() = default;

    /**
     * @brief Coleta estatísticas em tempo real (carga, freq, temp).
     */
    virtual models::CpuStats collect_stats() = 0;

    /**
     * @brief Coleta informações estáticas de arquitetura (modelo, cores, caches).
     */
    virtual models::CpuArchitecture collect_architecture() = 0;
};

} // namespace my_comp::domain::interfaces
