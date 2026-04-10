#include "linux_hardware_provider.h"
#include <fstream>
#include <filesystem>
#include <QStringList>

namespace fs = std::filesystem;

namespace my_comp::infrastructure::collectors {

LinuxHardwareProvider::LinuxHardwareProvider(std::shared_ptr<IShellExecutor> executor)
    : m_executor(std::move(executor)) {}

domain::models::HardwareTopology LinuxHardwareProvider::collect_topology() {
    domain::models::HardwareTopology topology;
    collect_smbios(topology);
    collect_usb(topology);
    collect_bluetooth(topology);
    return topology;
}

void LinuxHardwareProvider::collect_smbios(domain::models::HardwareTopology& topology) {
    auto read_sys_file = [](const std::string& path) -> QString {
        if (fs::exists(path)) {
            std::ifstream file(path);
            std::string line;
            if (std::getline(file, line)) {
                return QString::fromStdString(line).trimmed();
            }
        }
        return "Unknown";
    };

    const std::string base_path = "/sys/class/dmi/id/";
    topology.smbios.vendor = read_sys_file(base_path + "sys_vendor");
    topology.smbios.model = read_sys_file(base_path + "product_name");
    topology.smbios.version = read_sys_file(base_path + "product_version");
    topology.smbios.serial = read_sys_file(base_path + "product_serial");
    topology.smbios.sku = read_sys_file(base_path + "product_sku");
    topology.smbios.boardName = read_sys_file(base_path + "board_name");
    topology.smbios.boardVendor = read_sys_file(base_path + "board_vendor");
    topology.smbios.boardVersion = read_sys_file(base_path + "board_version");
    topology.smbios.boardSerial = read_sys_file(base_path + "board_serial");
}

void LinuxHardwareProvider::collect_usb(domain::models::HardwareTopology& topology) {
    QString output;
    // lsusb format: Bus 001 Device 002: ID 046d:c52b Logitech, Inc. Unifying Receiver
    if (m_executor->execute("lsusb", {}, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            domain::models::UsbDevice dev;
            // Parsing básico: Bus XXX Device XXX: ID XXXX:XXXX Name
            QStringList parts = line.split(' ');
            if (parts.size() >= 6) {
                dev.bus = parts[1];
                dev.vendorId = parts[5].split(':').at(0);
                dev.productId = parts[5].split(':').at(1);
                dev.name = parts.mid(6).join(' ');
                topology.usbDevices.push_back(dev);
            }
        }
    }
}

void LinuxHardwareProvider::collect_bluetooth(domain::models::HardwareTopology& topology) {
    QString output;
    // bluetoothctl devices format: Device XX:XX:XX:XX:XX:XX Name
    if (m_executor->execute("bluetoothctl", {"devices"}, output) == 0) {
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            domain::models::BluetoothDevice dev;
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                dev.address = parts[1];
                dev.name = parts.mid(2).join(' ');
                dev.paired = true; // bluetoothctl devices lista pareados por padrao
                topology.bluetoothDevices.push_back(dev);
            }
        }
    }
}

} // namespace my_comp::infrastructure::collectors
