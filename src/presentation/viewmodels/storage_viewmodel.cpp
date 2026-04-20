#include "storage_viewmodel.h"
#include <QLocale>
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

StorageViewModel::StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &StorageViewModel::refresh);
}

void StorageViewModel::refresh() {
    if (!m_timer->isActive()) {
        m_timer->start(1000);
    }
    if (m_busy) return;
    m_busy = true;

    QPointer<StorageViewModel> self(this);
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

QString StorageViewModel::totalThroughput() const {
    double total = m_stats.total_read_mb_s + m_stats.total_write_mb_s;
    if (total < 0.1) return "0 MB/s";
    return QString("%1 MB/s").arg(QLocale().toString(total, 'f', 1));
}

} // namespace my_comp::presentation::viewmodels
