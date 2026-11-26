#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <chrono>
#include "../../domain/interfaces/i_network_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

struct NetworkSample {
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    std::chrono::steady_clock::time_point timestamp;
};

class LinuxNetworkProvider : public domain::interfaces::INetworkProvider {
public:
    explicit LinuxNetworkProvider(std::shared_ptr<IShellExecutor> shell);
    domain::models::NetworkStats collect_stats() override;
    void set_external_probe_enabled(bool enabled) override { m_external_probe_enabled = enabled; }

private:
    double measure_latency();
    
    std::shared_ptr<IShellExecutor> m_shell;
    std::map<std::string, NetworkSample> m_last_samples;
    // Escrito pela thread de UI, lido no worker de coleta.
    std::atomic<bool> m_external_probe_enabled{false};
};

} // namespace my_comp::infrastructure::collectors
