#pragma once
#include <vector>

namespace my_comp::domain::models {

struct CoreStat {
    int id;
    double load_percent;
    double frequency_mhz;
};

struct CpuStats {
    double total_load;
    double average_frequency;
    double temperature;
    std::vector<CoreStat> cores;
};

} // namespace my_comp::domain::models
