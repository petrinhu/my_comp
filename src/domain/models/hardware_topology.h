#pragma once
#include <QString>
#include <QStringList>
#include <vector>

namespace my_comp::domain::models {

struct UsbDevice {
    QString name;
    QString vendorId;
    QString productId;
    QString manufacturer;
    QString bus;
    QString level;
    QString port;
};

struct BluetoothDevice {
    QString name = "";
    QString address = "";
    bool paired = false;
    bool connected = false;
    QString type = "";
};

struct SmbiosInfo {
    QString vendor;
    QString model;
    QString version;
    QString serial;
    QString sku;
    QString assetTag;
    QString boardName;
    QString boardVendor;
    QString boardVersion;
    QString boardSerial;
    // Campos vazios = indisponíveis. Os listados aqui existem mas exigem root para leitura
    // (ex.: "serial", "boardSerial"), para a UI dizer "requer root" em vez de "—" (HW-01b).
    QStringList restrictedFields;
};

struct HardwareTopology {
    SmbiosInfo smbios;
    std::vector<UsbDevice> usbDevices;
    std::vector<BluetoothDevice> bluetoothDevices;
};

} // namespace my_comp::domain::models
