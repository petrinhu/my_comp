#pragma once
#include "../../domain/interfaces/i_gpu_provider.h"

namespace my_comp::infrastructure::collectors {

class SysfsGpuProvider : public domain::interfaces::IGpuProvider {
public:
    std::vector<domain::models::GpuStats> collect_stats() override;

private:
    uint64_t read_sysfs_value(const QString& path);
};

} // namespace my_comp::infrastructure::collectors
