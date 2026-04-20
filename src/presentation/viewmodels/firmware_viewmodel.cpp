#include "firmware_viewmodel.h"
#include <QPointer>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

namespace my_comp::presentation::viewmodels {

FirmwareViewModel::FirmwareViewModel(std::shared_ptr<domain::interfaces::IFirmwareProvider> provider, QObject *parent)
    : QObject(parent), m_provider(provider) {
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
    QPointer<FirmwareViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto result = provider->get_devices();

        // Simular atualização de contagem (no futuro usar check_updates)
        int updates = 0;
        for (const auto& dev : result) if (dev.has_update) updates++;

        QMetaObject::invokeMethod(self.data(), [self, result, updates]() {
            if (!self) return;
            self->m_devices = result;
            self->m_update_count = updates;
            self->m_is_busy = false;
            emit self->devicesChanged();
            emit self->busyChanged();
        }, Qt::QueuedConnection);
    });
}

} // namespace my_comp::presentation::viewmodels
