#include "gpu_viewmodel.h"
#include "../../infrastructure/collectors/nvidia_gpu_provider.h"
#include "../../infrastructure/collectors/sysfs_gpu_provider.h"
#include <QVariantMap>
#include <QProcess>

namespace my_comp::presentation::viewmodels {

GpuViewModel::GpuViewModel(std::shared_ptr<IShellExecutor> executor, QObject* parent) : QObject(parent) {
    // Tenta detectar NVIDIA primeiro via IShellExecutor
    QString output;
    if (executor->execute("which", {"nvidia-smi"}, output) == 0) {
        m_providers.push_back(std::make_unique<infrastructure::collectors::NvidiaGpuProvider>(executor));
    } else {
        m_providers.push_back(std::make_unique<infrastructure::collectors::SysfsGpuProvider>());
    }

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GpuViewModel::update_stats);
    m_timer->start(2000);
    
    update_stats();
}

GpuViewModel::GpuViewModel(std::vector<std::unique_ptr<IGpuProvider>> providers, QObject* parent)
    : QObject(parent), m_providers(std::move(providers)) {
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
