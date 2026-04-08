#pragma once
#include <QString>

namespace my_comp::domain::models {

struct PowerStats {
    double battery_percent = 0.0;
    double consumption_watts = 0.0;
    double health_percent = 0.0;
    int cycle_count = 0;
    QString status; // "Charging", "Discharging", "Full"
    bool is_ac_connected = false;
};

} // namespace my_comp::domain::models
