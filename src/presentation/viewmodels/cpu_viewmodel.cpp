#include "cpu_viewmodel.h"
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

CpuViewModel::CpuViewModel(std::shared_ptr<domain::interfaces::ICpuProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    // CLAUDE.md §4: ctor não faz I/O. Trabalho pesado vai para initialize().
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CpuViewModel::refresh);
}

void CpuViewModel::initialize() {
    if (m_initialized) return;
    m_initialized = true;

    if (!m_provider) return;

    // Arquitetura é estática (lida uma vez de /proc/cpuinfo e /sys/devices/system/cpu).
    m_arch = m_provider->collect_architecture();
    emit architectureChanged();

    refresh();
    m_timer->start(1000);
}

void CpuViewModel::refresh() {
    if (m_busy) return;
    if (!m_provider) return;
    m_busy = true;

    QPointer<CpuViewModel> self(this);
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

QVariantList CpuViewModel::coreLoads() const {
    QVariantList list;
    for (const auto& core : m_stats.cores) {
        QVariantMap map;
        map["id"] = core.id;
        map["load"] = core.load_percent;
        map["freq"] = core.frequency_mhz;
        list.append(map);
    }
    return list;
}

} // namespace my_comp::presentation::viewmodels
