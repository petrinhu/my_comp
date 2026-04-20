#pragma once
#include "../models/smart_report.h"

namespace my_comp::domain::interfaces {

class ISmartProvider {
public:
    virtual ~ISmartProvider() = default;

    // `device` deve ser o nome curto (ex: "nvme0n1", "sda") — a implementação prefixa "/dev/".
    virtual models::SmartReport collect_report(const QString& device) = 0;
};

} // namespace my_comp::domain::interfaces
