#include "dbus_firmware_provider.h"
#include <QtDBus/QDBusMessage>
#include <QVariant>
#include <QDebug>
#include <QtDBus/QDBusArgument>

namespace my_comp::infrastructure::collectors {

DBusFirmwareProvider::DBusFirmwareProvider()
    : m_interface(std::make_unique<QDBusInterface>(
        "org.freedesktop.fwupd",
        "/",
        "org.freedesktop.fwupd",
        QDBusConnection::systemBus())) {}

std::vector<domain::models::FirmwareDevice> DBusFirmwareProvider::get_devices() {
    std::vector<domain::models::FirmwareDevice> devices;
    
    if (!m_interface->isValid()) {
        qWarning() << "[Firmware] D-Bus interface is not valid:" << m_interface->lastError().message();
        return devices;
    }

    QDBusMessage msg = m_interface->call("GetDevices");
    if (msg.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "[Firmware] Failed to get devices:" << msg.errorMessage();
        return devices;
    }

    if (msg.arguments().isEmpty()) {
        qWarning() << "[Firmware] Empty reply from GetDevices";
        return devices;
    }

    QVariant arg0 = msg.arguments().at(0);
    if (!arg0.canConvert<QDBusArgument>()) {
        qWarning() << "[Firmware] Unexpected return type:" << arg0.typeName();
        return devices;
    }

    QDBusArgument dbusArg = arg0.value<QDBusArgument>();
    if (dbusArg.currentType() != QDBusArgument::ArrayType) {
        qWarning() << "[Firmware] Not an array!";
        return devices;
    }

    dbusArg.beginArray();
    while (!dbusArg.atEnd()) {
        QMap<QString, QVariant> map;
        dbusArg >> map;
        
        domain::models::FirmwareDevice dev;
        dev.id = map.value("DeviceId").toString();
        dev.name = map.value("Name").toString();
        dev.vendor = map.value("Vendor").toString();
        dev.version = map.value("Version").toString();
        
        if (dev.name.isEmpty()) dev.name = dev.id.left(8);
        
        devices.push_back(dev);
    }
    dbusArg.endArray();

    return devices;
}

void DBusFirmwareProvider::check_updates(const QString& device_id) {
    if (!m_interface->isValid()) return;
    
    // GetUpgrades requer um DeviceId. Retorna uma lista de releases (aa{sv}).
    m_interface->call("GetUpgrades", device_id);
}

} // namespace my_comp::infrastructure::collectors
