#pragma once
#include <vector>
#include <QString>

namespace my_comp::domain::models {

struct InterfaceStat {
    QString name;
    double rx_mbps = 0.0;
    double tx_mbps = 0.0;
    bool is_up = false;
};

struct NetworkStats {
    std::vector<InterfaceStat> interfaces;
    double total_rx_mbps = 0.0;
    double total_tx_mbps = 0.0;
    double latency_ms = -1.0; // < 0 = não medida (sonda externa desligada ou falhou)
    QString active_interface;
};

} // namespace my_comp::domain::models
