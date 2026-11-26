#include "sysfs_gpu_provider.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

namespace {

QString read_first_line(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QTextStream(&file).readLine().trimmed();
}

QString vendor_name(const QString& vendor_id) {
    if (vendor_id == "0x1002") return "AMD";
    if (vendor_id == "0x8086") return "Intel";
    if (vendor_id == "0x10de") return "NVIDIA";
    return "Unknown";
}

double read_hwmon_temp(const QString& device_path) {
    const QDir hwmon_dir(device_path + "/hwmon");
    if (!hwmon_dir.exists()) return 0.0;
    const auto hwmons = hwmon_dir.entryList({"hwmon*"}, QDir::Dirs);
    for (const auto& h : hwmons) {
        const QString raw = read_first_line(device_path + "/hwmon/" + h + "/temp1_input");
        bool ok = false;
        const long long millideg = raw.toLongLong(&ok);
        if (ok && millideg > 0) return static_cast<double>(millideg) / 1000.0;
    }
    return 0.0;
}

} // namespace

std::vector<domain::models::GpuStats> SysfsGpuProvider::collect_stats() {
    std::vector<domain::models::GpuStats> gpus;

    const QDir drm_dir(m_drm_root);
    if (!drm_dir.exists()) return gpus;

    const QRegularExpression card_re("^card\\d+$");
    const QStringList entries = drm_dir.entryList({"card*"}, QDir::Dirs);

    for (const QString& card : entries) {
        if (!card_re.match(card).hasMatch()) continue;

        const QString device_path = m_drm_root + "/" + card + "/device";
        const QString vendor_id = read_first_line(device_path + "/vendor");
        if (vendor_id.isEmpty()) continue;

        // NVIDIA é coberto pelo NvidiaGpuProvider via nvidia-smi
        if (vendor_id == "0x10de") continue;

        domain::models::GpuStats stats;
        stats.is_active = true;
        stats.vendor = vendor_name(vendor_id);
        stats.model_name = card;

        bool ok = false;
        const double busy = read_first_line(device_path + "/gpu_busy_percent").toDouble(&ok);
        if (ok) stats.load_percent = busy;

        constexpr uint64_t kMiB = 1024ULL * 1024ULL;
        const uint64_t vram_total_bytes = read_first_line(device_path + "/mem_info_vram_total").toULongLong(&ok);
        if (ok) stats.vram_total_mb = vram_total_bytes / kMiB;

        const uint64_t vram_used_bytes = read_first_line(device_path + "/mem_info_vram_used").toULongLong(&ok);
        if (ok) stats.vram_used_mb = vram_used_bytes / kMiB;

        if (stats.vram_total_mb > 0) {
            stats.vram_usage_percent =
                static_cast<double>(stats.vram_used_mb) / static_cast<double>(stats.vram_total_mb) * 100.0;
        }

        stats.temperature = read_hwmon_temp(device_path);

        gpus.push_back(stats);
    }

    return gpus;
}

} // namespace my_comp::infrastructure::collectors
