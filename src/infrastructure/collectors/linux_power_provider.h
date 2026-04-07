#pragma once
#include "../../domain/interfaces/i_power_provider.h"
#include <QString>

namespace my_comp::infrastructure::collectors {
class LinuxPowerProvider : public domain::interfaces::IPowerProvider {
public:
    std::vector<domain::models::PowerStats> get_power_data() override;
};
}
