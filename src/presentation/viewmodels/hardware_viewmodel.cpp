#include "hardware_viewmodel.h"
#include <QtConcurrent/QtConcurrent>
#include <QPointer>

namespace my_comp::presentation::viewmodels {

HardwareViewModel::HardwareViewModel(std::shared_ptr<domain::interfaces::IHardwareProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    // MC-04: ctor não faz I/O. HardwareProvider::collect_topology spawna lsusb + bluetoothctl.
}

void HardwareViewModel::initialize() {
    if (m_initialized) return;
    m_initialized = true;
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
    // Re-entrância: collect_topology roda lsusb + bluetoothctl (subprocess), pode durar ~1s.
    // Sem este guard, cada refresh do sequenciador/UI enfileira um job extra.
    if (m_busy) return;
    m_busy = true;

    QPointer<HardwareViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto data = provider->collect_topology();
        QMetaObject::invokeMethod(self.data(), [self, data = std::move(data)]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_data = std::move(data);
            vm->m_busy = false;
            emit vm->dataChanged();
        }, Qt::QueuedConnection);
    });
}

}
