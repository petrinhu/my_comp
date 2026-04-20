#include "nvidia_gpu_provider.h"
#include <QStringList>
#include <algorithm>
#include <iterator>

namespace my_comp::infrastructure::collectors {

NvidiaGpuProvider::NvidiaGpuProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

std::vector<domain::models::GpuStats> NvidiaGpuProvider::collect_stats() {
    std::vector<domain::models::GpuStats> results;
    QString output;
    
    QStringList args;
    args << "--query-gpu=gpu_name,utilization.gpu,temperature.gpu,memory.total,memory.used"
         << "--format=csv,noheader,nounits";

    if (m_executor->execute("nvidia-smi", args, output) != 0) return results;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    std::transform(lines.begin(), lines.end(), std::back_inserter(results), 
                   [this](const QString& line) { return parse_line(line); });

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
            stats.vram_usage_percent = (100.0 * static_cast<double>(stats.vram_used_mb)) / static_cast<double>(stats.vram_total_mb);
        }
    }

    return stats;
}

} // namespace my_comp::infrastructure::collectors
