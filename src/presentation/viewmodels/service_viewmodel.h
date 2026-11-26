#pragma once
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "../../domain/interfaces/i_service_provider.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

class ServiceViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList services READ services NOTIFY servicesChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)

public:
    explicit ServiceViewModel(std::shared_ptr<domain::interfaces::IServiceProvider> provider, QObject* parent = nullptr);

    QVariantList services() const;
    bool isBusy() const { return m_isBusy; }
    QString sortField() const { return m_sortField; }
    void setSortField(const QString& field);
    bool sortAscending() const { return m_sortAscending; }
    void setSortAscending(bool asc);

    Q_INVOKABLE void refresh();
    // scope: "system" ou "user" (campo `scope` de cada item de `services`).
    Q_INVOKABLE void enableAndStart(const QString& name, const QString& scope);  // persiste + liga
    Q_INVOKABLE void stopNow(const QString& name, const QString& scope);         // só esta sessão
    Q_INVOKABLE void disableAndStop(const QString& name, const QString& scope);  // persiste + desliga
    Q_INVOKABLE void createDaemon(const QString& exe_path, const QString& params);

signals:
    void servicesChanged();
    void isBusyChanged();
    void sortFieldChanged();
    void sortAscendingChanged();
    void operationFinished(bool success, const QString& message);

private:
    void setBusy(bool busy);
    void rebuildProcessedList();
    using Action = domain::models::OperationResult (domain::interfaces::IServiceProvider::*)(
        const QString&, domain::models::ServiceScope);
    void run_action(Action action, const QString& name, const QString& scope);
    std::shared_ptr<domain::interfaces::IServiceProvider> m_provider;
    std::vector<domain::models::ServiceInfo> m_data;
    QVariantList m_processedList;
    bool m_isBusy = false;
    QString m_sortField = "name";
    bool m_sortAscending = true;
};

}
