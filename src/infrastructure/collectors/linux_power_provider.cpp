#include "linux_power_provider.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace my_comp::infrastructure::collectors {

std::vector<domain::models::PowerStats> LinuxPowerProvider::get_power_data() {
    std::vector<domain::models::PowerStats> stats;
    QDir powerDir("/sys/class/power_supply");
    QStringList supplies = powerDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& supply : supplies) {
        if (supply.startsWith("BAT")) {
            domain::models::PowerStats s;
            s.model = supply.toStdString();
            
            QFile energy(powerDir.absoluteFilePath(supply + "/energy_now"));
            if (energy.open(QIODevice::ReadOnly)) {
                s.power_watts = energy.readAll().toDouble() / 1e6; // Simplificação para Watts
            }

            QFile capacity(powerDir.absoluteFilePath(supply + "/capacity"));
            if (capacity.open(QIODevice::ReadOnly)) {
                s.battery_percent = capacity.readAll().toDouble();
            }

            QFile status(powerDir.absoluteFilePath(supply + "/status"));
            if (status.open(QIODevice::ReadOnly)) {
                s.is_charging = status.readAll().trimmed() == "Charging";
            }
            stats.push_back(s);
        }
    }
    return stats;
}
}
