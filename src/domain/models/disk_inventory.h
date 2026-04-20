#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

struct Partition {
    QString name;
    QString mount;
    QString fs;
    qint64 size_bytes = 0;
    QString label;
    QString uuid;
    qint64 used_bytes = 0;
    qint64 free_bytes = 0;
    double used_percent = 0.0;
};

struct DiskDrive {
    QString name;
    qint64 size_bytes = 0;
    QString type;
    QString vendor;
    QString model;
    bool rotational = false;
    std::vector<Partition> partitions;
};

struct DiskInventory {
    std::vector<DiskDrive> drives;
};

} // namespace my_comp::domain::models
