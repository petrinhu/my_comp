#pragma once
#include "../models/benchmark_result.h"

namespace my_comp::domain::interfaces {

class IBenchmarkProvider {
public:
    virtual ~IBenchmarkProvider() = default;

    // `device` é o nome curto (ex.: "sda"); implementação prefixa "/dev/".
    virtual models::BenchmarkResult run(const QString& device) = 0;
};

} // namespace my_comp::domain::interfaces
