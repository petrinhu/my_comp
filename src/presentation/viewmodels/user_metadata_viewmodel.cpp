#include "user_metadata_viewmodel.h"

#include <QDebug>
#include <QMetaObject>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include <exception>

namespace my_comp::presentation::viewmodels {

UserMetadataViewModel::UserMetadataViewModel(std::shared_ptr<domain::interfaces::IUserMetadataProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
}

QVariantMap UserMetadataViewModel::envVars() const {
    QVariantMap map;
    auto it = m_data.envVars.begin();
    while (it != m_data.envVars.end()) {
        map.insert(it.key(), it.value());
        ++it;
    }
    return map;
}

// UX-05: get_user_metadata roda last e ulimit e percorre getpwent/getgrgid_r, que podem
// ir ao NSS/LDAP: fora da thread da UI. Uma falha não pode deixar m_busy preso (QA-13).
void UserMetadataViewModel::refresh() {
    if (!m_provider || m_busy) return;
    m_busy = true;

    const QPointer<UserMetadataViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        domain::models::UserMetadata data;
        try {
            data = provider->get_user_metadata();
        } catch (const std::exception& e) {
            qWarning() << "[UserMetadataVM] falha na coleta:" << e.what();
        } catch (...) {
            qWarning() << "[UserMetadataVM] falha desconhecida na coleta";
        }
        QMetaObject::invokeMethod(self.data(), [self, data = std::move(data)]() mutable {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_data = std::move(data);
            vm->m_busy = false;
            emit vm->dataChanged();
        }, Qt::QueuedConnection);
    });
}

}
