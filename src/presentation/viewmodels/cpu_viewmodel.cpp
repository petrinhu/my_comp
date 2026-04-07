#include "cpu_viewmodel.h"
#include <QVariantMap>

namespace my_comp::presentation::viewmodels {

CpuViewModel::CpuViewModel(std::shared_ptr<domain::interfaces::ICpuProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    
    // Coleta inicial de arquitetura (estática)
    m_arch = m_provider->collect_architecture();
    
    // Timer para atualização periódica (1s)
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CpuViewModel::refresh);
    m_timer->start(1000);
    
    // Primeira coleta
    refresh();
}

void CpuViewModel::refresh() {
    m_stats = m_provider->collect_stats();
    emit statsChanged();
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
