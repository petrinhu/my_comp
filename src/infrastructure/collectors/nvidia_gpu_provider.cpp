#include "nvidia_gpu_provider.h"
#include <QProcess>
#include <QStringList>

namespace my_comp::infrastructure::collectors {

std::vector<GpuStats> NvidiaGpuProvider::get_all_gpus() {
    std::vector<GpuStats> gpus;
    QProcess process;
    
    // Comando para retornar dados da GPU em formato CSV (sem cabeçalho)
    // nvidia-smi --query-gpu=gpu_name,utilization.gpu,temperature.gpu,memory.used,memory.total,power.draw,clocks.current.graphics,fan.speed --format=csv,noheader,nounits
    QStringList args;
    args << "--query-gpu=gpu_name,utilization.gpu,temperature.gpu,memory.used,memory.total,power.draw,clocks.current.graphics,fan.speed"
         << "--format=csv,noheader,nounits";

    process.start("nvidia-smi", args);
    if (!process.waitForFinished()) return gpus;

    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        gpus.push_back(parse_nvidia_smi_line(line));
    }

    return gpus;
}

GpuStats NvidiaGpuProvider::parse_nvidia_smi_line(const QString& line) {
    QStringList fields = line.split(',', Qt::KeepEmptyParts);
    GpuStats stats{};
    stats.vendor = "NVIDIA";

    if (fields.size() >= 8) {
        stats.model = fields[0].trimmed().toStdString();
        stats.load = fields[1].trimmed().toUInt();
        stats.temp = fields[2].trimmed().toUInt();
        stats.vram_used = fields[3].trimmed().toUInt();
        stats.vram_total = fields[4].trimmed().toUInt();
        stats.power_usage = static_cast<uint32_t>(fields[5].trimmed().toFloat()); // Pode ser decimal
        stats.clock_speed = fields[6].trimmed().toUInt();
        stats.fan_speed = fields[7].trimmed().toUInt();
    }

    return stats;
}

} // namespace my_comp::infrastructure::collectors
