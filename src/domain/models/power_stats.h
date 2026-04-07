#pragma once

#include "../../domain/interfaces/i_power_provider.h"
#include <vector>
#include <memory>
#include <string>

namespace my_comp::domain::models {
struct PowerStats {
    double power_watts;      // Consumo atual em Watts
    double battery_percent;  // Nível de carga (0-100)
    bool is_charging;        // Estado de carga
    std::string model;       // Ex: BAT0
};
}
