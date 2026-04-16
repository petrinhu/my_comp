#include "service_viewmodel.h"
#include <QtConcurrent>

namespace my_comp::presentation::viewmodels {

ServiceViewModel::ServiceViewModel(std::shared_ptr<domain::interfaces::IServiceProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
}

QVariantList ServiceViewModel::services() const {
    QVariantList list;
    for (const auto& info : m_data) {
        QVariantMap map;
        map["name"] = info.name;
        map["description"] = info.description;
        map["loadState"] = info.loadState;
        map["activeState"] = info.activeState;
        map["subState"] = info.subState;
        map["isEnabled"] = info.isEnabled;
        list.append(map);
    }
    return list;
}

void ServiceViewModel::setBusy(bool busy) {
    if (m_isBusy != busy) {
        m_isBusy = busy;
        emit isBusyChanged();
    }
}

void ServiceViewModel::refresh() {
    if (m_isBusy) return;
    setBusy(true);
    
    (void)QtConcurrent::run(
[this]() {
        auto result = m_provider->get_services();
        QMetaObject::invokeMethod(this, [this, result]() {
            m_data = result;
            setBusy(false);
            emit servicesChanged();
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::startService(const QString& name) {
    setBusy(true);
    (void)QtConcurrent::run(
[this, name]() {
        bool ok = m_provider->start_service(name);
        QMetaObject::invokeMethod(this, [this, ok, name]() {
            setBusy(false);
            if (ok) refresh();
            emit operationFinished(ok, ok ? "Serviço " + name + " iniciado." : "Falha ao iniciar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::pauseService(const QString& name) {
    setBusy(true);
    (void)QtConcurrent::run(
[this, name]() {
        bool ok = m_provider->pause_service(name);
        QMetaObject::invokeMethod(this, [this, ok, name]() {
            setBusy(false);
            if (ok) refresh();
            emit operationFinished(ok, ok ? "Serviço " + name + " pausado (sessão)." : "Falha ao pausar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::stopService(const QString& name) {
    setBusy(true);
    (void)QtConcurrent::run(
[this, name]() {
        bool ok = m_provider->stop_service(name);
        QMetaObject::invokeMethod(this, [this, ok, name]() {
            setBusy(false);
            if (ok) refresh();
            emit operationFinished(ok, ok ? "Serviço " + name + " parado de vez." : "Falha ao parar " + name);
        }, Qt::QueuedConnection);
    });
}

void ServiceViewModel::createDaemon(const QString& exe_path, const QString& params) {
    setBusy(true);
    (void)QtConcurrent::run(
[this, exe_path, params]() {
        bool ok = m_provider->create_daemon(exe_path, params);
        QMetaObject::invokeMethod(this, [this, ok]() {
            setBusy(false);
            if (ok) refresh();
            emit operationFinished(ok, ok ? "Daemon criado e iniciado com sucesso." : "Falha ao criar daemon.");
        }, Qt::QueuedConnection);
    });
}

}
