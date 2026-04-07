#include "storage_viewmodel.h"
#include <QLocale>

namespace my_comp::presentation::viewmodels {

StorageViewModel::StorageViewModel(std::shared_ptr<domain::interfaces::IStorageProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &StorageViewModel::refresh);
    m_timer->start(1000);
    
    refresh();
}

void StorageViewModel::refresh() {
    m_stats = m_provider->collect_stats();
    emit statsChanged();
}

QString StorageViewModel::totalThroughput() const {
    double total = m_stats.total_read_mb_s + m_stats.total_write_mb_s;
    if (total < 0.1) return "0 MB/s";
    return QString("%1 MB/s").arg(QLocale().toString(total, 'f', 1));
}

} // namespace my_comp::presentation::viewmodels
