#pragma once

#include "../../domain/interfaces/i_gpu_provider.h"
#include <QString>

namespace my_comp::infrastructure::collectors {

using namespace my_comp::domain::interfaces;

class SysfsGpuProvider : public IGpuProvider {
public:
    SysfsGpuProvider() = default;
    ~SysfsGpuProvider() override = default;

    std::vector<GpuStats> get_all_gpus() override;

private:
    // Auxiliar para ler valores do sysfs
    uint64_t read_sysfs_value(const QString& path);
    QString read_sysfs_string(const QString& path);
};

} // namespace my_comp::infrastructure::collectors
