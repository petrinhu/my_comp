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

public:
    explicit ServiceViewModel(std::shared_ptr<domain::interfaces::IServiceProvider> provider, QObject* parent = nullptr);

    QVariantList services() const;
    bool isBusy() const { return m_isBusy; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void startService(const QString& name);
    Q_INVOKABLE void pauseService(const QString& name);
    Q_INVOKABLE void stopService(const QString& name);
    Q_INVOKABLE void createDaemon(const QString& exe_path, const QString& params);

signals:
    void servicesChanged();
    void isBusyChanged();
    void operationFinished(bool success, const QString& message);

private:
    void setBusy(bool busy);
    std::shared_ptr<domain::interfaces::IServiceProvider> m_provider;
    std::vector<domain::models::ServiceInfo> m_data;
    QVariantList m_processedList;
    bool m_isBusy = false;
};

}
