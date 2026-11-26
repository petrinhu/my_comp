#pragma once
#include <QObject>
#include <QTimer>
#include <memory>
#include "../../domain/interfaces/i_network_provider.h"

namespace my_comp::presentation::viewmodels {

class NetworkViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(double totalRx READ totalRx NOTIFY statsChanged)
    Q_PROPERTY(double totalTx READ totalTx NOTIFY statsChanged)
    Q_PROPERTY(double latency READ latency NOTIFY statsChanged)
    Q_PROPERTY(QString activeInterface READ activeInterface NOTIFY statsChanged)
    Q_PROPERTY(QString status READ status NOTIFY statsChanged)
    // MC-10: ping a host externo só com opt-in explícito; persistido em QSettings.
    Q_PROPERTY(bool externalProbeEnabled READ externalProbeEnabled WRITE setExternalProbeEnabled NOTIFY externalProbeEnabledChanged)

public:
    explicit NetworkViewModel(std::shared_ptr<domain::interfaces::INetworkProvider> provider, QObject *parent = nullptr);

    double totalRx() const { return m_stats.total_rx_mbps; }
    double totalTx() const { return m_stats.total_tx_mbps; }
    double latency() const { return m_stats.latency_ms; }
    QString activeInterface() const { return m_stats.active_interface; }
    bool externalProbeEnabled() const { return m_external_probe_enabled; }
    void setExternalProbeEnabled(bool enabled);
    QString status() const { 
        if (m_stats.latency_ms > 0) return "Online";
        if (m_stats.total_rx_mbps > 0 || m_stats.total_tx_mbps > 0) return "Local Only";
        return "Offline";
    }

public slots:
    void refresh();

signals:
    void statsChanged();
    void externalProbeEnabledChanged();

private:
    std::shared_ptr<domain::interfaces::INetworkProvider> m_provider;
    domain::models::NetworkStats m_stats;
    QTimer *m_timer;
    bool m_busy = false;
    bool m_external_probe_enabled = false;
    bool m_preferences_loaded = false;
    void load_preferences();
};

} // namespace my_comp::presentation::viewmodels
