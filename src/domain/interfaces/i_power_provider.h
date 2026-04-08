#pragma once
#include "../models/power_stats.h"

namespace my_comp::domain::interfaces {

class IPowerProvider {
public:
    virtual ~IPowerProvider() = default;

    /**
     * @brief Coleta estatísticas de bateria e energia.
     */
    virtual models::PowerStats collect_stats() = 0;
};

} // namespace my_comp::domain::interfaces
