#include "sysfs_gpu_provider.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

namespace my_comp::infrastructure::collectors {

std::vector<domain::models::GpuStats> SysfsGpuProvider::collect_stats() {
    std::vector<domain::models::GpuStats> gpus;
    
    // Procura por GPUs em /sys/class/drm/cardX/device/
    QDir drm_dir("/sys/class/drm");
    QStringList cards = drm_dir.entryList({"card*"}, QDir::Dirs);

    for (const QString& card : cards) {
        QString path = "/sys/class/drm/" + card + "/device/";
        if (!QFile::exists(path + "vendor")) continue;

        domain::models::GpuStats stats;
        stats.is_active = true;
        
        // Exemplo simples para Intel/AMD via sysfs
        // Muitos drivers não expõe carga em tempo real de forma simples no sysfs
        // Para o Alpha, focaremos na detecção básica
        stats.vendor = "Integrated/Generic";
        stats.model_name = card;
        
        gpus.push_back(stats);
    }

    return gpus;
}

uint64_t SysfsGpuProvider::read_sysfs_value(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QTextStream in(&file);
    return in.readLine().toULongLong();
}

} // namespace my_comp::infrastructure::collectors
