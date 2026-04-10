#include "hardware_viewmodel.h"

namespace my_comp::presentation::viewmodels {

HardwareViewModel::HardwareViewModel(std::shared_ptr<domain::interfaces::IHardwareProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    refresh();
}

QVariantMap HardwareViewModel::smbios() const {
    QVariantMap map;
    map["vendor"] = m_data.smbios.vendor;
    map["model"] = m_data.smbios.model;
    map["version"] = m_data.smbios.version;
    map["serial"] = m_data.smbios.serial;
    map["sku"] = m_data.smbios.sku;
    map["boardName"] = m_data.smbios.boardName;
    map["boardVendor"] = m_data.smbios.boardVendor;
    map["boardVersion"] = m_data.smbios.boardVersion;
    map["boardSerial"] = m_data.smbios.boardSerial;
    return map;
}

QVariantList HardwareViewModel::usbDevices() const {
    QVariantList list;
    for (const auto& dev : m_data.usbDevices) {
        QVariantMap map;
        map["name"] = dev.name;
        map["vendorId"] = dev.vendorId;
        map["productId"] = dev.productId;
        map["bus"] = dev.bus;
        list.append(map);
    }
    return list;
}

QVariantList HardwareViewModel::bluetoothDevices() const {
    QVariantList list;
    for (const auto& dev : m_data.bluetoothDevices) {
        QVariantMap map;
        map["name"] = dev.name;
        map["address"] = dev.address;
        map["paired"] = dev.paired;
        list.append(map);
    }
    return list;
}

void HardwareViewModel::refresh() {
    m_data = m_provider->collect_topology();
    emit dataChanged();
}

}
