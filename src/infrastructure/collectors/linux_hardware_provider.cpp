#include "linux_hardware_provider.h"
#include <QRegularExpression>
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

void LinuxHardwareProvider::parse_lsusb_output(const QString& output, std::vector<domain::models::UsbDevice>& devices) {
    // lsusb format: Bus 001 Device 002: ID 046d:c52b Logitech, Inc. Unifying Receiver
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        const QStringList parts = line.split(' ');
        if (parts.size() < 6) continue;
        const QStringList vidpid = parts[5].split(':');
        if (vidpid.size() < 2) continue; // protege contra entrada malformada (BT-02 fuzz)
        domain::models::UsbDevice dev;
        dev.bus = parts[1];
        dev.vendorId = vidpid.at(0);
        dev.productId = vidpid.at(1);
        dev.name = parts.mid(6).join(' ');
        devices.push_back(dev);
    }
}

namespace {
// MAC vai para o argv do `bluetoothctl info`: só endereço bem formado passa (MC-10).
bool is_valid_mac(const QString& s) {
    static const QRegularExpression mac(QStringLiteral("^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$"));
    return mac.match(s).hasMatch();
}
} // namespace

void LinuxHardwareProvider::parse_bluetoothctl_output(const QString& output, std::vector<domain::models::BluetoothDevice>& devices) {
    // `bluetoothctl devices`: "Device XX:XX:XX:XX:XX:XX Nome". Lista também dispositivos só
    // conhecidos (não pareados), então paired/connected vêm do `info` (HW-02b).
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3 || parts[0] != QLatin1String("Device") || !is_valid_mac(parts[1])) continue;
        domain::models::BluetoothDevice dev;
        dev.address = parts[1];
        dev.name = parts.mid(2).join(' ');
        devices.push_back(dev);
    }
}

void LinuxHardwareProvider::parse_bluetoothctl_info(const QString& output, domain::models::BluetoothDevice& device) {
    // Linhas "\tChave: valor"; só as que mudam o estado exibido.
    for (const QString& raw : output.split('\n', Qt::SkipEmptyParts)) {
        const QString line = raw.trimmed();
        const auto colon = line.indexOf(QLatin1Char(':'));
        if (colon <= 0) continue;
        const QString key = line.left(colon);
        const QString value = line.mid(colon + 1).trimmed();
        if (key == QLatin1String("Paired")) device.paired = (value == QLatin1String("yes"));
        else if (key == QLatin1String("Connected")) device.connected = (value == QLatin1String("yes"));
        else if (key == QLatin1String("Icon")) device.type = value;
    }
}

void LinuxHardwareProvider::collect_usb(domain::models::HardwareTopology& topology) {
    QString output;
    if (m_executor->execute("lsusb", {}, output) == 0) {
        parse_lsusb_output(output, topology.usbDevices);
    }
}

void LinuxHardwareProvider::collect_bluetooth(domain::models::HardwareTopology& topology) {
    QString output;
    if (m_executor->execute("bluetoothctl", {"devices"}, output) == 0) {
        parse_bluetoothctl_output(output, topology.bluetoothDevices);
    }
    // Roda na thread de coleta do HardwareViewModel (QtConcurrent), não na UI.
    for (auto& dev : topology.bluetoothDevices) {
        QString info;
        if (m_executor->execute("bluetoothctl", {"info", dev.address}, info) == 0) {
            parse_bluetoothctl_info(info, dev);
        }
    }
}

} // namespace my_comp::infrastructure::collectors
