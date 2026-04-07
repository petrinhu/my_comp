#pragma once
#include "../models/memory_stats.h"

namespace my_comp::domain::interfaces {

class IMemoryProvider {
public:
    virtual ~IMemoryProvider() = default;

    /**
     * @brief Coleta estatísticas em tempo real de RAM, Swap e ZRAM.
     */
    virtual models::MemoryStats collect_stats() = 0;
};

} // namespace my_comp::domain::interfaces
