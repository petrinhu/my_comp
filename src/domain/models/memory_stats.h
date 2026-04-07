#pragma once
#include <cstdint>

namespace my_comp::domain::models {

struct MemoryStats {
    uint64_t ram_total_kb = 0;
    uint64_t ram_free_kb = 0;
    uint64_t ram_available_kb = 0;
    uint64_t ram_used_kb = 0;
    double ram_usage_percent = 0.0;
    
    uint64_t swap_total_kb = 0;
    uint64_t swap_free_kb = 0;
    uint64_t swap_used_kb = 0;
    double swap_usage_percent = 0.0;
    
    bool has_zram = false;
    double zram_compression_ratio = 1.0;
};

} // namespace my_comp::domain::models
