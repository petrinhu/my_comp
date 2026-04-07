#pragma once
#include <fstream>
#include <string>
#include <regex>
#include "../../domain/interfaces/i_memory_provider.h"

namespace my_comp::infrastructure::collectors {

class LinuxMemoryProvider : public domain::interfaces::IMemoryProvider {
public:
    domain::models::MemoryStats collect_stats() override;

private:
    uint64_t parse_kb(const std::string& line);
    void update_zram(domain::models::MemoryStats& stats);
};

} // namespace my_comp::infrastructure::collectors
