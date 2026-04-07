#pragma once
#include <QString>

namespace my_comp::domain::models {

struct FirmwareDevice {
    QString id;
    QString name;
    QString vendor;
    QString version;
    bool has_update = false;
    QString latest_version;
    int update_urgency = 0; // 1 = Low, 2 = Medium, 3 = High, 4 = Critical
};

} // namespace my_comp::domain::models
