#pragma once
#include <memory>
#include "../../domain/interfaces/i_benchmark_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

class HdparmBenchmarkProvider : public domain::interfaces::IBenchmarkProvider {
public:
    explicit HdparmBenchmarkProvider(std::shared_ptr<IShellExecutor> shell)
        : m_shell(std::move(shell)) {}

    domain::models::BenchmarkResult run(const QString& device) override;

private:
    std::shared_ptr<IShellExecutor> m_shell;
};

} // namespace my_comp::infrastructure::collectors
