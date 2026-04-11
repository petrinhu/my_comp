#pragma once
#include <string>
#include <vector>

namespace my_comp::domain::models {

struct CacheInfo {
    std::string level;
    std::string type;
    std::string size;
};

struct CpuArchitecture {
    std::string model_name = "";
    std::string vendor_id = "";
    int physical_cores = 0;
    int logical_threads = 0;
    std::vector<CacheInfo> caches = {};
    std::string l1_cache = "";
    std::string l2_cache = "";
    std::string l3_cache = "";
};

} // namespace my_comp::domain::models
