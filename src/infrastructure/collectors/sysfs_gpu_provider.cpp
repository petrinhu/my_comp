#include "sysfs_gpu_provider.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace my_comp::infrastructure::collectors {

std::vector<GpuStats> SysfsGpuProvider::get_all_gpus() {
    std::vector<GpuStats> gpus;
    QDir drmDir("/sys/class/drm");
    QStringList cards = drmDir.entryList({"card[0-9]"}, QDir::Dirs);

    for (const QString& card : cards) {
        QString devicePath = drmDir.absoluteFilePath(card) + "/device";
        if (!QFile::exists(devicePath + "/vendor")) continue;

        QString vendorId = read_sysfs_string(devicePath + "/vendor").trimmed();
        
        // Intel (0x8086) ou AMD (0x1002)
        if (vendorId != "0x8086" && vendorId != "0x1002") continue;

        GpuStats stats{};
        stats.vendor = (vendorId == "0x8086") ? "Intel" : "AMD";
        stats.model = (stats.vendor == "Intel") ? "Integrated Graphics" : "Radeon Graphics";

        // Coleta de Temperatura (exemplo básico via hwmon associado)
        QDir hwmonDir(devicePath + "/hwmon");
        if (hwmonDir.exists()) {
            QStringList hwmons = hwmonDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            if (!hwmons.empty()) {
                QString tempPath = hwmonDir.absoluteFilePath(hwmons[0]) + "/temp1_input";
                stats.temp = read_sysfs_value(tempPath) / 1000;
            }
        }

        // Carga (Intel/AMD possuem arquivos diferentes, ex: gpu_busy_percent no i915)
        QString loadPath = devicePath + "/gpu_busy_percent"; // AMD
        if (!QFile::exists(loadPath)) loadPath = devicePath + "/gt_usage_pct"; // Intel alguns drivers
        
        if (QFile::exists(loadPath)) {
            stats.load = read_sysfs_value(loadPath);
        }

        gpus.push_back(stats);
    }

    return gpus;
}

uint64_t SysfsGpuProvider::read_sysfs_value(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        return QTextStream(&file).readAll().trimmed().toULongLong();
    }
    return 0;
}

QString SysfsGpuProvider::read_sysfs_string(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        return QTextStream(&file).readAll().trimmed();
    }
    return "";
}

} // namespace my_comp::infrastructure::collectors
