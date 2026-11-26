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

    /**
     * @brief Liga/desliga a sonda de latência que sai da rede local (MC-10: opt-in,
     * desligada por padrão). Com ela desligada, latency_ms < 0.
     */
    virtual void set_external_probe_enabled(bool enabled) { (void)enabled; }
};

} // namespace my_comp::domain::interfaces
