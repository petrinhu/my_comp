#include "network_viewmodel.h"
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

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
    if (m_busy) return;
    m_busy = true;

    QPointer<NetworkViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto stats = provider->collect_stats();
        QMetaObject::invokeMethod(self.data(), [self, stats]() {
            if (!self) return;
            self->m_stats = stats;
            self->m_busy = false;
            emit self->statsChanged();
        }, Qt::QueuedConnection);
    });
}

} // namespace my_comp::presentation::viewmodels
