#include "user_metadata_viewmodel.h"

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

void UserMetadataViewModel::refresh() {
    if (!m_provider) return;
    m_data = m_provider->get_user_metadata();
    emit dataChanged();
}

}
