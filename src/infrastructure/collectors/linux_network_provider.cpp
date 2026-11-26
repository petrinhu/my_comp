#include "linux_network_provider.h"
#include <sstream>
#include <regex>
#include <iostream>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

LinuxNetworkProvider::LinuxNetworkProvider(std::shared_ptr<IShellExecutor> shell)
    : m_shell(std::move(shell)) {
    // Captura inicial para baseline de tráfego, sem usar a chamada virtual collect_stats()
    std::ifstream netdev("/proc/net/dev");
    std::string line;
    auto now = std::chrono::steady_clock::now();
    std::getline(netdev, line);
    std::getline(netdev, line);
    while (std::getline(netdev, line)) {
        std::stringstream ss(line);
        std::string iface_raw; ss >> iface_raw;
        if (iface_raw.empty()) continue;
        std::string iface_name = iface_raw.substr(0, iface_raw.find(':'));
        uint64_t rx_bytes, rx_packets, rx_errs, rx_drop, rx_fifo, rx_frame, rx_compressed, rx_multi, tx_bytes;
        ss >> rx_bytes >> rx_packets >> rx_errs >> rx_drop >> rx_fifo >> rx_frame >> rx_compressed >> rx_multi >> tx_bytes;
        m_last_samples[iface_name] = {rx_bytes, tx_bytes, now};
    }
}

domain::models::NetworkStats LinuxNetworkProvider::collect_stats() {
    domain::models::NetworkStats stats;
    std::ifstream netdev("/proc/net/dev");
    std::string line;
    auto now = std::chrono::steady_clock::now();
    
    // Pula as duas primeiras linhas de cabeçalho
    std::getline(netdev, line);
    std::getline(netdev, line);
    
    while (std::getline(netdev, line)) {
        std::stringstream ss(line);
        std::string iface_raw;
        ss >> iface_raw;
        if (iface_raw.empty()) continue;
        
        std::string iface_name = iface_raw.substr(0, iface_raw.find(':'));
        if (iface_name == "lo") continue;
        
        uint64_t rx_bytes, rx_packets, rx_errs, rx_drop, rx_fifo, rx_frame, rx_compressed, rx_multi;
        uint64_t tx_bytes;
        
        ss >> rx_bytes >> rx_packets >> rx_errs >> rx_drop >> rx_fifo >> rx_frame >> rx_compressed >> rx_multi >> tx_bytes;
        
        if (m_last_samples.count(iface_name)) {
            const auto& last = m_last_samples[iface_name];
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last.timestamp).count();
            
            if (duration > 0) {
                double seconds = static_cast<double>(duration) / 1000.0;
                double rx_mbps = (static_cast<double>(rx_bytes - last.rx_bytes) * 8.0) / (1024.0 * 1024.0 * seconds);
                double tx_mbps = (static_cast<double>(tx_bytes - last.tx_bytes) * 8.0) / (1024.0 * 1024.0 * seconds);
                
                domain::models::InterfaceStat istat;
                istat.name = QString::fromStdString(iface_name);
                istat.rx_mbps = rx_mbps;
                istat.tx_mbps = tx_mbps;
                istat.is_up = true; // Simplificação: se está no proc/net/dev e tem tráfego, está up
                
                stats.interfaces.push_back(istat);
                
                // Determinar interface ativa (a que tem mais tráfego acumulado ou atual)
                if (rx_mbps > 0.01 || tx_mbps > 0.01) {
                   if (stats.active_interface.isEmpty() || rx_mbps > stats.total_rx_mbps) {
                       stats.active_interface = istat.name;
                   }
                }
                
                stats.total_rx_mbps += rx_mbps;
                stats.total_tx_mbps += tx_mbps;
            }
        }
        
        m_last_samples[iface_name] = {rx_bytes, tx_bytes, now};
    }
    
    stats.latency_ms = m_external_probe_enabled ? measure_latency() : -1.0;
    
    return stats;
}

double LinuxNetworkProvider::measure_latency() {
    QString output;
    // Só roda com opt-in do usuário (MC-10): pinga o DNS público 1.1.1.1.
    // Sem rede, falha e devolve -1 (não medida).
    QStringList args = {"-c", "1", "-W", "1", "1.1.1.1"};
    if (m_shell->execute("ping", args, output) == 0) {
        QRegularExpression regex("(?:time|tempo)=([\\d.]+) ms");
        auto match = regex.match(output);
        if (match.hasMatch()) {
            return match.captured(1).toDouble();
        }
    }
    return -1.0;
}

} // namespace my_comp::infrastructure::collectors
