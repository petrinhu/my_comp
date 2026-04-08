#include "linux_power_provider.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <cmath>

namespace my_comp::infrastructure::collectors {

domain::models::PowerStats LinuxPowerProvider::collect_stats() {
    domain::models::PowerStats stats;
    
    // Identifica o diretório da bateria (BAT1 no seu Acer)
    QString bat_path = "/sys/class/power_supply/BAT1/";
    if (!QDir(bat_path).exists()) bat_path = "/sys/class/power_supply/BAT0/";
    
    if (QDir(bat_path).exists()) {
        stats.battery_percent = read_sysfs(bat_path + "capacity").toDouble();
        stats.status = read_sysfs(bat_path + "status").trimmed();
        stats.cycle_count = read_sysfs(bat_path + "cycle_count").toInt();
        
        double voltage = read_sysfs(bat_path + "voltage_now").toDouble() / 1000000.0; // uV to V
        double current = read_sysfs(bat_path + "current_now").toDouble() / 1000000.0; // uA to A
        stats.consumption_watts = std::abs(voltage * current);
        
        double full = read_sysfs(bat_path + "charge_full").toDouble();
        double design = read_sysfs(bat_path + "charge_full_design").toDouble();
        if (design > 0) {
            stats.health_percent = (full / design) * 100.0;
        }
    }
    
    // AC status
    QString ac_path = "/sys/class/power_supply/ACAD/online";
    if (QFile::exists(ac_path)) {
        stats.is_ac_connected = (read_sysfs(ac_path).trimmed() == "1");
    }

    return stats;
}

QString LinuxPowerProvider::read_sysfs(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "0";
    return file.readAll();
}

} // namespace my_comp::infrastructure::collectors
