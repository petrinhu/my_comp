#include "memory_viewmodel.h"
#include <QLocale>

namespace my_comp::presentation::viewmodels {

MemoryViewModel::MemoryViewModel(std::shared_ptr<domain::interfaces::IMemoryProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MemoryViewModel::refresh);
    m_timer->start(2000); // 2 segundos para memória é suficiente e economiza CPU
    
    refresh();
}

void MemoryViewModel::refresh() {
    m_stats = m_provider->collect_stats();
    emit statsChanged();
}

QString MemoryViewModel::format_kb(uint64_t kb) const {
    double value = static_cast<double>(kb);
    QString unit = "KB";
    
    if (kb > 1024 * 1024) {
        value /= (1024.0 * 1024.0);
        unit = "GB";
    } else if (kb > 1024) {
        value /= 1024.0;
        unit = "MB";
    }
    
    return QString("%1 %2").arg(QLocale().toString(value, 'f', 1), unit);
}

} // namespace my_comp::presentation::viewmodels
