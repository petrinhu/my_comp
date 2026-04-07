#include "gpu_viewmodel.h"
#include "../../infrastructure/collectors/nvidia_gpu_provider.h"
#include "../../infrastructure/collectors/sysfs_gpu_provider.h"
#include <QVariantMap>

namespace my_comp::presentation::viewmodels {

using namespace my_comp::infrastructure::collectors;

GpuViewModel::GpuViewModel(QObject* parent) : QObject(parent) {
    // Adiciona provedores (Orquestrador simples na ViewModel para agilidade, 
    // mas seguindo a abstração de interfaces).
    m_providers.push_back(std::make_unique<NvidiaGpuProvider>());
    m_providers.push_back(std::make_unique<SysfsGpuProvider>());

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GpuViewModel::update_stats);
    m_timer->start(2000); // Atualiza a cada 2 segundos

    update_stats();
}

void GpuViewModel::update_stats() {
    QVariantList newGpus;
    for (const auto& provider : m_providers) {
        auto statsList = provider->get_all_gpus();
        for (const auto& stats : statsList) {
            QVariantMap map;
            map["vendor"] = QString::fromStdString(stats.vendor);
            map["model"] = QString::fromStdString(stats.model);
            map["load"] = stats.load;
            map["temp"] = stats.temp;
            map["vram_used"] = stats.vram_used;
            map["vram_total"] = stats.vram_total;
            map["power_usage"] = stats.power_usage;
            map["clock_speed"] = stats.clock_speed;
            map["fan_speed"] = stats.fan_speed;
            newGpus.append(map);
        }
    }

    if (m_gpus != newGpus) {
        m_gpus = newGpus;
        emit gpusChanged();
    }
}

} // namespace my_comp::presentation::viewmodels
