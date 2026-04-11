#pragma once
#include <vector>

namespace my_comp::domain::models {

struct CoreStat {
    int id = 0;
    double load_percent = 0.0;
    double frequency_mhz = 0.0;
};

struct CpuStats {
    double total_load = 0.0;
    double average_frequency = 0.0;
    double temperature = 0.0;
    std::vector<CoreStat> cores;
};

} // namespace my_comp::domain::models
