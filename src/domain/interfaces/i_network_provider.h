#pragma once
#include "../models/network_stats.h"

namespace my_comp::domain::interfaces {

class INetworkProvider {
public:
    virtual ~INetworkProvider() = default;

    /**
     * @brief Coleta estatísticas de tráfego e latência em tempo real.
     */
    virtual models::NetworkStats collect_stats() = 0;
};

} // namespace my_comp::domain::interfaces
