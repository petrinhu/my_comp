#pragma once
#include <QString>
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
    QString name;
    QString address;
    bool paired;
    bool connected;
    QString type;
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
};

struct HardwareTopology {
    SmbiosInfo smbios;
    std::vector<UsbDevice> usbDevices;
    std::vector<BluetoothDevice> bluetoothDevices;
};

} // namespace my_comp::domain::models
