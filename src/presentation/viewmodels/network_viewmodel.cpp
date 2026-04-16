#include "network_viewmodel.h"

namespace my_comp::presentation::viewmodels {

NetworkViewModel::NetworkViewModel(std::shared_ptr<domain::interfaces::INetworkProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &NetworkViewModel::refresh);
}

void NetworkViewModel::refresh() {
    if (!m_timer->isActive()) {
        m_timer->start(1500);
    }
    m_stats = m_provider->collect_stats();
    emit statsChanged();
}

} // namespace my_comp::presentation::viewmodels
