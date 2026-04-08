#pragma once
#include "../../domain/interfaces/i_power_provider.h"
#include <QString>

namespace my_comp::infrastructure::collectors {

class LinuxPowerProvider : public domain::interfaces::IPowerProvider {
public:
    domain::models::PowerStats collect_stats() override;

private:
    QString read_sysfs(const QString& path);
};

} // namespace my_comp::infrastructure::collectors
