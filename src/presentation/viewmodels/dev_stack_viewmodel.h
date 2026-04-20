#pragma once
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "../../domain/interfaces/i_dev_stack_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class DevStackViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList toolchains READ toolchains NOTIFY dataChanged)
    Q_PROPERTY(QVariantList containers READ containers NOTIFY dataChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)

public:
    explicit DevStackViewModel(std::shared_ptr<domain::interfaces::IDevStackProvider> provider, QObject* parent = nullptr);

    QVariantList toolchains() const;
    QVariantList containers() const;
    bool isBusy() const { return m_isBusy; }
    QString sortField() const { return m_sortField; }
    void setSortField(const QString& f);
    bool sortAscending() const { return m_sortAscending; }
    void setSortAscending(bool a);

    Q_INVOKABLE void refresh();

signals:
    void dataChanged();
    void isBusyChanged();
    void sortFieldChanged();
    void sortAscendingChanged();

private:
    std::shared_ptr<domain::interfaces::IDevStackProvider> m_provider;
    domain::models::DevStackInfo m_data;
    bool m_isBusy = false;
    QString m_sortField{"name"};
    bool m_sortAscending{true};
};

}
