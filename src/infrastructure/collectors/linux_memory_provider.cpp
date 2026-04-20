#include "linux_memory_provider.h"
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

namespace my_comp::infrastructure::collectors {

domain::models::MemoryStats LinuxMemoryProvider::collect_stats() {
    domain::models::MemoryStats stats;
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 8, "MemTotal") == 0) stats.ram_total_kb = parse_kb(line);
        else if (line.compare(0, 7, "MemFree") == 0) stats.ram_free_kb = parse_kb(line);
        else if (line.compare(0, 12, "MemAvailable") == 0) stats.ram_available_kb = parse_kb(line);
        else if (line.compare(0, 9, "SwapTotal") == 0) stats.swap_total_kb = parse_kb(line);
        else if (line.compare(0, 8, "SwapFree") == 0) stats.swap_free_kb = parse_kb(line);
    }
    
    // Cálculos de RAM
    if (stats.ram_total_kb > 0) {
        // Used = Total - Available (melhor aproximação que Total - Free)
        stats.ram_used_kb = stats.ram_total_kb - stats.ram_available_kb;
        stats.ram_usage_percent = (100.0 * static_cast<double>(stats.ram_used_kb)) / static_cast<double>(stats.ram_total_kb);
    }
    
    // Cálculos de Swap
    if (stats.swap_total_kb > 0) {
        stats.swap_used_kb = stats.swap_total_kb - stats.swap_free_kb;
        stats.swap_usage_percent = (100.0 * static_cast<double>(stats.swap_used_kb)) / static_cast<double>(stats.swap_total_kb);
    }
    
    update_zram(stats);
    
    return stats;
}

uint64_t LinuxMemoryProvider::parse_kb(const std::string& line) {
    std::regex regex("(\\d+)");
    std::smatch match;
    if (std::regex_search(line, match, regex)) {
        return std::stoull(match[1]);
    }
    return 0;
}

void LinuxMemoryProvider::update_zram(domain::models::MemoryStats& stats) {
    if (fs::exists("/sys/block/zram0/mm_stat")) {
        std::ifstream mm_stat("/sys/block/zram0/mm_stat");
        uint64_t orig_size, compr_size;
        if (mm_stat >> orig_size >> compr_size) {
            stats.has_zram = true;
            if (compr_size > 0) {
                stats.zram_compression_ratio = static_cast<double>(orig_size) / static_cast<double>(compr_size);
            } else {
                stats.zram_compression_ratio = 1.0;
            }
        }
    }
}

} // namespace my_comp::infrastructure::collectors
