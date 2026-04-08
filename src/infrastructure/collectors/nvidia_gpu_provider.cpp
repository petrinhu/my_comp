#include "nvidia_gpu_provider.h"
#include <QProcess>
#include <QStringList>

namespace my_comp::infrastructure::collectors {

std::vector<domain::models::GpuStats> NvidiaGpuProvider::collect_stats() {
    std::vector<domain::models::GpuStats> results;
    QProcess process;
    
    QStringList args;
    args << "--query-gpu=gpu_name,utilization.gpu,temperature.gpu,memory.total,memory.used"
         << "--format=csv,noheader,nounits";

    process.start("nvidia-smi", args);
    if (!process.waitForFinished()) return results;

    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        results.push_back(parse_line(line));
    }

    return results;
}

domain::models::GpuStats NvidiaGpuProvider::parse_line(const QString& line) {
    QStringList fields = line.split(',', Qt::KeepEmptyParts);
    domain::models::GpuStats stats;
    stats.vendor = "NVIDIA";
    stats.is_active = true;

    if (fields.size() >= 5) {
        stats.model_name = fields[0].trimmed();
        stats.load_percent = fields[1].trimmed().toDouble();
        stats.temperature = fields[2].trimmed().toDouble();
        stats.vram_total_mb = fields[3].trimmed().toULongLong();
        stats.vram_used_mb = fields[4].trimmed().toULongLong();
        
        if (stats.vram_total_mb > 0) {
            stats.vram_usage_percent = (100.0 * stats.vram_used_mb) / stats.vram_total_mb;
        }
    }

    return stats;
}

} // namespace my_comp::infrastructure::collectors
