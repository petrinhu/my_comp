#include "service_viewmodel.h"
#include <QPointer>
#include <QtConcurrent>
#include <algorithm>

namespace my_comp::presentation::viewmodels {

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
        newList.append(map);
    }
    m_processedList = newList;
    emit servicesChanged();
}

void ServiceViewModel::refresh() {
    if (m_isBusy) return;
    setBusy(true);

    QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider]() {
        auto result = provider->get_services();

        QMetaObject::invokeMethod(self.data(), [self, result]() {
            if (!self) return;
            self->m_data = result;
            self->setBusy(false);
            self->rebuildProcessedList();
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::startService(const QString& name) {
    setBusy(true);
    QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, name]() {
        bool ok = provider->start_service(name);
        QMetaObject::invokeMethod(self.data(), [self, ok, name]() {
            if (!self) return;
            self->setBusy(false);
            if (ok) self->refresh();
            emit self->operationFinished(ok, ok ? "Serviço " + name + " iniciado." : "Falha ao iniciar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::pauseService(const QString& name) {
    setBusy(true);
    QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, name]() {
        bool ok = provider->pause_service(name);
        QMetaObject::invokeMethod(self.data(), [self, ok, name]() {
            if (!self) return;
            self->setBusy(false);
            if (ok) self->refresh();
            emit self->operationFinished(ok, ok ? "Serviço " + name + " pausado (sessão)." : "Falha ao pausar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::stopService(const QString& name) {
    setBusy(true);
    QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, name]() {
        bool ok = provider->stop_service(name);
        QMetaObject::invokeMethod(self.data(), [self, ok, name]() {
            if (!self) return;
            self->setBusy(false);
            if (ok) self->refresh();
            emit self->operationFinished(ok, ok ? "Serviço " + name + " parado de vez." : "Falha ao parar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::createDaemon(const QString& exe_path, const QString& params) {
    setBusy(true);
    QPointer<ServiceViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, exe_path, params]() {
        bool ok = provider->create_daemon(exe_path, params);
        QMetaObject::invokeMethod(self.data(), [self, ok]() {
            if (!self) return;
            self->setBusy(false);
            if (ok) self->refresh();
            emit self->operationFinished(ok, ok ? "Daemon criado e iniciado com sucesso." : "Falha ao criar daemon.");
        }, Qt::QueuedConnection);
    });
}

}
