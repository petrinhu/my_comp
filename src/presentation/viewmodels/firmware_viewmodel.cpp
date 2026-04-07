#include "firmware_viewmodel.h"
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

namespace my_comp::presentation::viewmodels {

FirmwareViewModel::FirmwareViewModel(std::shared_ptr<domain::interfaces::IFirmwareProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
    refresh();
}

QVariantList FirmwareViewModel::devices() const {
    QVariantList list;
    for (const auto& dev : m_devices) {
        QVariantMap map;
        map["id"] = dev.id;
        map["name"] = dev.name;
        map["vendor"] = dev.vendor;
        map["version"] = dev.version;
        map["hasUpdate"] = dev.has_update;
        map["latestVersion"] = dev.latest_version;
        map["urgency"] = dev.update_urgency;
        list.append(map);
    }
    return list;
}

void FirmwareViewModel::refresh() {
    if (m_is_busy) return;
    
    m_is_busy = true;
    emit busyChanged();

    // Executar em thread separada para não travar a UI (especialmente DBus pode demorar)
    QtConcurrent::run([this]() {
        auto result = m_provider->get_devices();
        
        // Simular atualização de contagem (no futuro usar check_updates)
        int updates = 0;
        for (const auto& dev : result) if (dev.has_update) updates++;

        QMetaObject::invokeMethod(this, [this, result, updates]() {
            m_devices = result;
            m_update_count = updates;
            m_is_busy = false;
            emit devicesChanged();
            emit busyChanged();
        });
    });
}

} // namespace my_comp::presentation::viewmodels
