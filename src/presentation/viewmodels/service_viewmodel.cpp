#include "service_viewmodel.h"
#include <QPointer>
#include <QtConcurrent>
#include <algorithm>

namespace my_comp::presentation::viewmodels {

namespace {
domain::models::ServiceScope parse_scope(const QString& scope) {
    return scope == QLatin1String("user") ? domain::models::ServiceScope::User
                                          : domain::models::ServiceScope::System;
}
} // namespace

ServiceViewModel::ServiceViewModel(std::shared_ptr<domain::interfaces::IServiceProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
}

QVariantList ServiceViewModel::services() const {
    return m_processedList;
}

void ServiceViewModel::setBusy(bool busy) {
    if (m_isBusy != busy) {
        m_isBusy = busy;
        emit isBusyChanged();
    }
}

void ServiceViewModel::setSortField(const QString& field) {
    if (m_sortField != field) {
        m_sortField = field;
        emit sortFieldChanged();
        rebuildProcessedList();
    }
}

void ServiceViewModel::setSortAscending(bool asc) {
    if (m_sortAscending != asc) {
        m_sortAscending = asc;
        emit sortAscendingChanged();
        rebuildProcessedList();
    }
}

void ServiceViewModel::rebuildProcessedList() {
    auto sorted = m_data;
    const QString field = m_sortField;
    const bool asc = m_sortAscending;
    std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
        if (field == "active") return asc ? (a.activeState < b.activeState) : (a.activeState > b.activeState);
        if (field == "description") return asc ? (a.description < b.description) : (a.description > b.description);
        return asc ? (a.name < b.name) : (a.name > b.name);
    });
    QVariantList newList;
    for (const auto& info : sorted) {
        QVariantMap map;
        map["name"] = info.name;
        map["description"] = info.description;
        map["loadState"] = info.loadState;
        map["activeState"] = info.activeState;
        map["subState"] = info.subState;
        map["isEnabled"] = info.isEnabled;
        map["scope"] = (info.scope == domain::models::ServiceScope::User) ? QStringLiteral("user") : QStringLiteral("system");
        newList.append(map);
    }
    m_processedList = newList;
    emit servicesChanged();
}

void ServiceViewModel::refresh() {
    if (m_isBusy) return;
    setBusy(true);

    const QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto result = provider->get_services();

        QMetaObject::invokeMethod(self.data(), [self, result]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_data = result;
            vm->setBusy(false);
            vm->rebuildProcessedList();
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::run_action(Action action, const QString& name, const QString& scope) {
    setBusy(true);
    const QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    const auto service_scope = parse_scope(scope);
    (void)QtConcurrent::run([self, provider, action, name, service_scope]() {
        const auto result = ((*provider).*action)(name, service_scope);
        QMetaObject::invokeMethod(self.data(), [self, result]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->setBusy(false);
            if (result.ok) vm->refresh();
            emit vm->operationFinished(result.ok, result.message);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::enableAndStart(const QString& name, const QString& scope) {
    run_action(&domain::interfaces::IServiceProvider::enable_and_start, name, scope);
}

void ServiceViewModel::stopNow(const QString& name, const QString& scope) {
    run_action(&domain::interfaces::IServiceProvider::stop, name, scope);
}

void ServiceViewModel::disableAndStop(const QString& name, const QString& scope) {
    run_action(&domain::interfaces::IServiceProvider::disable_and_stop, name, scope);
}

void ServiceViewModel::createDaemon(const QString& exe_path, const QString& params) {
    setBusy(true);
    const QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, exe_path, params]() {
        const auto result = provider->create_daemon(exe_path, params);
        QMetaObject::invokeMethod(self.data(), [self, result]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->setBusy(false);
            if (result.ok) vm->refresh();
            // Mensagem concreta do provider (SV-03c): diz ao usuário o que corrigir.
            emit vm->operationFinished(result.ok, result.message);
        }, Qt::QueuedConnection);
    });
}

}
