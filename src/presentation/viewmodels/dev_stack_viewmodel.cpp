#include "dev_stack_viewmodel.h"
#include <QtConcurrent>
#include <algorithm>

namespace my_comp::presentation::viewmodels {

DevStackViewModel::DevStackViewModel(std::shared_ptr<domain::interfaces::IDevStackProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
}

QVariantList DevStackViewModel::toolchains() const {
    auto sorted = m_data.toolchains;
    const QString field = m_sortField;
    const bool asc = m_sortAscending;
    std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
        if (field == "version") return asc ? (a.version < b.version) : (a.version > b.version);
        return asc ? (a.name < b.name) : (a.name > b.name);
    });
    QVariantList list;
    for (const auto& t : sorted) {
        QVariantMap map;
        map["name"] = t.name;
        map["version"] = t.version;
        map["isInstalled"] = t.isInstalled;
        list.append(map);
    }
    return list;
}

QVariantList DevStackViewModel::containers() const {
    auto sorted = m_data.containers;
    const QString field = m_sortField;
    const bool asc = m_sortAscending;
    std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
        if (field == "status") return asc ? (a.status < b.status) : (a.status > b.status);
        if (field == "image")  return asc ? (a.image < b.image)   : (a.image > b.image);
        return asc ? (a.name < b.name) : (a.name > b.name);
    });
    QVariantList list;
    for (const auto& c : sorted) {
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

void DevStackViewModel::setSortField(const QString& f) {
    if (m_sortField != f) {
        m_sortField = f;
        emit sortFieldChanged();
        emit dataChanged();
    }
}

void DevStackViewModel::setSortAscending(bool a) {
    if (m_sortAscending != a) {
        m_sortAscending = a;
        emit sortAscendingChanged();
        emit dataChanged();
    }
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
