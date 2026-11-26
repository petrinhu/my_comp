#include "memory_viewmodel.h"
#include <QLocale>
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

MemoryViewModel::MemoryViewModel(std::shared_ptr<domain::interfaces::IMemoryProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MemoryViewModel::refresh);
}

void MemoryViewModel::refresh() {
    if (!m_timer->isActive()) {
        m_timer->start(2000);
    }
    if (m_busy) return;
    m_busy = true;

    QPointer<MemoryViewModel> self(this);
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

QString MemoryViewModel::format_kb(uint64_t kb) const {
    double value = static_cast<double>(kb);
    QString unit = "KB";
    
    if (kb > 1024ULL * 1024ULL) {
        value /= (1024.0 * 1024.0);
        unit = "GB";
    } else if (kb > 1024) {
        value /= 1024.0;
        unit = "MB";
    }
    
    return QString("%1 %2").arg(QLocale().toString(value, 'f', 1), unit);
}

} // namespace my_comp::presentation::viewmodels
