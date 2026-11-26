#include "linux_hardware_provider.h"
#include <QRegularExpression>
#include <QFile>
#include <QStringList>

namespace my_comp::infrastructure::collectors {

LinuxHardwareProvider::LinuxHardwareProvider(std::shared_ptr<IShellExecutor> executor, QString dmi_root)
    : m_executor(std::move(executor)), m_dmi_root(std::move(dmi_root)) {}

domain::models::HardwareTopology LinuxHardwareProvider::collect_topology() {
    domain::models::HardwareTopology topology;
    collect_smbios(topology);
    collect_usb(topology);
    collect_bluetooth(topology);
    return topology;
}

namespace {
// Textos que firmwares genéricos gravam no lugar do valor real: equivalem a "ausente".
bool is_oem_placeholder(const QString& v) {
    static const QStringList placeholders = {
        "to be filled by o.e.m.", "default string", "not specified", "not applicable",
        "system product name", "system manufacturer", "system version", "system serial number",
        "none", "n/a", "x.x", "0", "00000000", "0123456789", "123456789",
    };
    return placeholders.contains(v.trimmed().toLower());
}
} // namespace

void LinuxHardwareProvider::collect_smbios(domain::models::HardwareTopology& topology) {
    auto& smbios = topology.smbios;
    // HW-01b: ausente/placeholder → vazio (a UI mostra "—"); existe mas sem permissão
    // (serial é 0400 root) → vazio + nome em restrictedFields (a UI mostra "requer root").
    auto read_field = [this, &smbios](const QString& file, const QString& field) -> QString {
        QFile f(m_dmi_root + "/" + file);
        if (!f.exists()) return {};
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            smbios.restrictedFields << field;
            return {};
        }
        const QString value = QString::fromUtf8(f.readLine()).trimmed();
        return is_oem_placeholder(value) ? QString() : value;
    };

    smbios.vendor = read_field("sys_vendor", "vendor");
    smbios.model = read_field("product_name", "model");
    smbios.version = read_field("product_version", "version");
    smbios.serial = read_field("product_serial", "serial");
    smbios.sku = read_field("product_sku", "sku");
    smbios.boardName = read_field("board_name", "boardName");
    smbios.boardVendor = read_field("board_vendor", "boardVendor");
    smbios.boardVersion = read_field("board_version", "boardVersion");
    smbios.boardSerial = read_field("board_serial", "boardSerial");
}

void LinuxHardwareProvider::parse_lsusb_output(const QString& output, std::vector<domain::models::UsbDevice>& devices) {
    // "Bus 001 Device 002: ID 046d:c52b Logitech, Inc. Unifying Receiver" (HW-02c).
    // Regex ancorada no formato em vez de split(' ') por posição: espaços extras não
    // deslocam colunas e linhas fora do formato (avisos do libusb, `lsusb -t`) são ignoradas.
    static const QRegularExpression line_re(QStringLiteral(
        "^\\s*Bus\\s+(\\d+)\\s+Device\\s+\\d+:\\s+ID\\s+([0-9A-Fa-f]{4}):([0-9A-Fa-f]{4})(?:\\s+(.*?))?\\s*$"));
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        const auto m = line_re.match(line);
        if (!m.hasMatch()) continue;
        domain::models::UsbDevice dev;
        dev.bus = m.captured(1);
        dev.vendorId = m.captured(2).toLower();
        dev.productId = m.captured(3).toLower();
        dev.name = m.captured(4);
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
