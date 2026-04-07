#pragma once
#include "../models/storage_stats.h"

namespace my_comp::domain::interfaces {

class IStorageProvider {
public:
    virtual ~IStorageProvider() = default;

    /**
     * @brief Coleta estatísticas de I/O em tempo real.
     */
    virtual models::StorageStats collect_stats() = 0;
};

} // namespace my_comp::domain::interfaces
