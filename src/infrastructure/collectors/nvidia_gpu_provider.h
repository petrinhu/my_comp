#pragma once

#include "../../domain/interfaces/i_gpu_provider.h"
#include <QStringList>

namespace my_comp::infrastructure::collectors {

using namespace my_comp::domain::interfaces;

class NvidiaGpuProvider : public IGpuProvider {
public:
    NvidiaGpuProvider() = default;
    ~NvidiaGpuProvider() override = default;

    std::vector<GpuStats> get_all_gpus() override;

private:
    // Método auxiliar para converter saída do nvidia-smi para struct GpuStats
    GpuStats parse_nvidia_smi_line(const QString& line);
};

} // namespace my_comp::infrastructure::collectors
