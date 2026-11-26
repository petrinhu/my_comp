#include "network_viewmodel.h"
#include <QtConcurrent/QtConcurrent>
#include <QPointer>
#include <QSettings>

namespace my_comp::presentation::viewmodels {

NetworkViewModel::NetworkViewModel(std::shared_ptr<domain::interfaces::INetworkProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &NetworkViewModel::refresh);
}

namespace {
const QString EXTERNAL_PROBE_KEY = QStringLiteral("network/external_latency_probe");
} // namespace

void NetworkViewModel::load_preferences() {
    // Fora do ctor (MC-04: sem I/O no construtor); roda na primeira coleta.
    m_preferences_loaded = true;
    m_external_probe_enabled = QSettings().value(EXTERNAL_PROBE_KEY, false).toBool();
    if (m_provider) m_provider->set_external_probe_enabled(m_external_probe_enabled);
    if (m_external_probe_enabled) emit externalProbeEnabledChanged();
}

void NetworkViewModel::setExternalProbeEnabled(bool enabled) {
    m_preferences_loaded = true;
    if (m_external_probe_enabled == enabled) return;
    m_external_probe_enabled = enabled;
    if (m_provider) m_provider->set_external_probe_enabled(enabled);
    QSettings().setValue(EXTERNAL_PROBE_KEY, enabled);
    emit externalProbeEnabledChanged();
}

void NetworkViewModel::refresh() {
    if (!m_preferences_loaded) load_preferences();
    if (!m_timer->isActive()) {
        m_timer->start(1500);
    }
    if (m_busy) return;
    m_busy = true;

    QPointer<NetworkViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto stats = provider->collect_stats();
        QMetaObject::invokeMethod(self.data(), [self, stats]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_stats = stats;
            vm->m_busy = false;
            emit vm->statsChanged();
        }, Qt::QueuedConnection);
    });
}

} // namespace my_comp::presentation::viewmodels
