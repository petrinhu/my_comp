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

    const QDBusMessage msg = m_interface->call("GetDevices");
    if (msg.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "[Firmware] Failed to get devices:" << msg.errorMessage();
        return devices;
    }

    if (msg.arguments().isEmpty()) {
        qWarning() << "[Firmware] Empty reply from GetDevices";
        return devices;
    }

    const QVariant arg0 = msg.arguments().at(0);
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
        // FWUPD retorna uma lista de dicionários: aa{sv}
        // Cada elemento é um QDBusArgument que representa o dicionário {sv}
        QVariant element;
        dbusArg >> element;
        
        if (element.canConvert<QDBusArgument>()) {
            QDBusArgument mapArg = element.value<QDBusArgument>();
            if (mapArg.currentType() == QDBusArgument::MapType) {
                QMap<QString, QVariant> map;
                mapArg.beginMap();
                while (!mapArg.atEnd()) {
                    QString key;
                    QVariant value;
                    mapArg.beginMapEntry();
                    mapArg >> key >> value;
                    mapArg.endMapEntry();
                    map[key] = value;
                }
                mapArg.endMap();

                domain::models::FirmwareDevice dev;
                dev.id = map.value("DeviceId").toString();
                dev.name = map.value("Name").toString();
                dev.vendor = map.value("Vendor").toString();
                dev.version = map.value("Version").toString();
                
                if (dev.name.isEmpty()) dev.name = dev.id.left(8);
                devices.push_back(dev);
            }
        }
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
