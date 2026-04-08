#include "gpu_viewmodel.h"
#include "../../infrastructure/collectors/nvidia_gpu_provider.h"
#include "../../infrastructure/collectors/sysfs_gpu_provider.h"
#include <QVariantMap>
#include <QProcess>

namespace my_comp::presentation::viewmodels {

GpuViewModel::GpuViewModel(QObject* parent) : QObject(parent) {
    // Tenta detectar NVIDIA primeiro
    QProcess checkProcess;
    checkProcess.start("which", {"nvidia-smi"});
    checkProcess.waitForFinished();
    
    if (checkProcess.exitCode() == 0) {
        m_providers.push_back(std::make_unique<infrastructure::collectors::NvidiaGpuProvider>());
    } else {
        m_providers.push_back(std::make_unique<infrastructure::collectors::SysfsGpuProvider>());
    }

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GpuViewModel::update_stats);
    m_timer->start(2000);
    
    update_stats();
}

void GpuViewModel::update_stats() {
    QVariantList newList;
    
    for (const auto& provider : m_providers) {
        auto stats_list = provider->collect_stats();
        for (const auto& stats : stats_list) {
            QVariantMap map;
            map["modelName"] = stats.model_name;
            map["vendor"] = stats.vendor;
            map["load"] = stats.load_percent;
            map["temp"] = stats.temperature;
            map["vramUsed"] = static_cast<double>(stats.vram_used_mb);
            map["vramTotal"] = static_cast<double>(stats.vram_total_mb);
            map["vramPercent"] = stats.vram_usage_percent;
            newList.append(map);
        }
    }

    if (m_gpus != newList) {
        m_gpus = newList;
        emit gpusChanged();
    }
}

} // namespace my_comp::presentation::viewmodels
