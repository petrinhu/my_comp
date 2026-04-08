#pragma once
#include <QString>

namespace my_comp::domain::models {

struct GpuStats {
    QString model_name;
    QString vendor;
    double load_percent = 0.0;
    double temperature = 0.0;
    uint64_t vram_total_mb = 0;
    uint64_t vram_used_mb = 0;
    double vram_usage_percent = 0.0;
    bool is_active = false;
};

} // namespace my_comp::domain::models
