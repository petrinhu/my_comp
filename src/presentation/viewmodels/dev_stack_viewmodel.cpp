#include "dev_stack_viewmodel.h"
#include <QtConcurrent>

namespace my_comp::presentation::viewmodels {

DevStackViewModel::DevStackViewModel(std::shared_ptr<domain::interfaces::IDevStackProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    refresh();
}

QVariantList DevStackViewModel::toolchains() const {
    QVariantList list;
    for (const auto& t : m_data.toolchains) {
        QVariantMap map;
        map["name"] = t.name;
        map["version"] = t.version;
        map["isInstalled"] = t.isInstalled;
        list.append(map);
    }
    return list;
}

QVariantList DevStackViewModel::containers() const {
    QVariantList list;
    for (const auto& c : m_data.containers) {
        QVariantMap map;
        map["id"] = c.id;
        map["name"] = c.name;
        map["image"] = c.image;
        map["status"] = c.status;
        map["runtime"] = c.runtime;
        list.append(map);
    }
    return list;
}

void DevStackViewModel::refresh() {
    if (m_isBusy) return;
    m_isBusy = true;
    emit isBusyChanged();

    (void)QtConcurrent::run([this]() {
        auto result = m_provider->collect_dev_stacks();
        QMetaObject::invokeMethod(this, [this, result]() {
            m_data = result;
            m_isBusy = false;
            emit isBusyChanged();
            emit dataChanged();
        });
    });
}

}
