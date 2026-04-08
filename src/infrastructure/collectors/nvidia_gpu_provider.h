#pragma once
#include "../../domain/interfaces/i_gpu_provider.h"

namespace my_comp::infrastructure::collectors {

class NvidiaGpuProvider : public domain::interfaces::IGpuProvider {
public:
    std::vector<domain::models::GpuStats> collect_stats() override;

private:
    domain::models::GpuStats parse_line(const QString& line);
};

} // namespace my_comp::infrastructure::collectors
