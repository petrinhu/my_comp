#pragma once
#include "../../domain/interfaces/i_gpu_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class NvidiaGpuProvider : public domain::interfaces::IGpuProvider {
public:
    explicit NvidiaGpuProvider(std::shared_ptr<IShellExecutor> executor);
    std::vector<domain::models::GpuStats> collect_stats() override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
    domain::models::GpuStats parse_line(const QString& line);
};

} // namespace my_comp::infrastructure::collectors
