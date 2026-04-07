#pragma once

#include <string>
#include <cstdint>

namespace my_comp::domain::models {

struct GpuStats {
    std::string vendor;      // NVIDIA, AMD, Intel
    std::string model;       // Ex: RTX 3050, Radeon RX 6600
    uint32_t load;           // %
    uint32_t temp;           // °C
    uint32_t vram_used;      // MB
    uint32_t vram_total;     // MB
    uint32_t power_usage;    // Watts
    uint32_t clock_speed;    // MHz
    uint32_t fan_speed;      // %
};

} // namespace my_comp::domain::models
