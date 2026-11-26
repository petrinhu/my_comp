#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

struct CacheInfo {
    QString level;
    QString type;
    QString size;
};

struct CpuArchitecture {
    QString model_name;
    QString vendor_id;
    int physical_cores = 0;
    int logical_threads = 0;
    std::vector<CacheInfo> caches;
    QString l1_cache;
    QString l2_cache;
    QString l3_cache;
};

} // namespace my_comp::domain::models
