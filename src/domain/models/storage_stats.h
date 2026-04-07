#pragma once
#include <vector>
#include <QString>

namespace my_comp::domain::models {

struct DriveStat {
    QString name;
    double read_mb_s = 0.0;
    double write_mb_s = 0.0;
    double util_percent = 0.0;
};

struct StorageStats {
    std::vector<DriveStat> drives;
    double total_read_mb_s = 0.0;
    double total_write_mb_s = 0.0;
};

} // namespace my_comp::domain::models
