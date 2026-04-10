#pragma once
#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include "../../domain/interfaces/i_user_metadata_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class UserMetadataViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY dataChanged)
    Q_PROPERTY(QString shell READ shell NOTIFY dataChanged)
    Q_PROPERTY(QString home READ home NOTIFY dataChanged)
    Q_PROPERTY(QStringList groups READ groups NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap envVars READ envVars NOTIFY dataChanged)
    Q_PROPERTY(QStringList systemLimits READ systemLimits NOTIFY dataChanged)
    Q_PROPERTY(QStringList loginHistory READ loginHistory NOTIFY dataChanged)
    Q_PROPERTY(QStringList systemUsers READ systemUsers NOTIFY dataChanged)

public:
    explicit UserMetadataViewModel(std::shared_ptr<domain::interfaces::IUserMetadataProvider> provider, QObject* parent = nullptr);

    QString currentUser() const { return m_data.currentUser; }
    QString shell() const { return m_data.shell; }
    QString home() const { return m_data.home; }
    QStringList groups() const { return m_data.groups; }
    QVariantMap envVars() const;
    QStringList systemLimits() const { return m_data.systemLimits; }
    QStringList loginHistory() const { return m_data.loginHistory; }
    QStringList systemUsers() const { return m_data.systemUsers; }

    Q_INVOKABLE void refresh();

signals:
    void dataChanged();

private:
    std::shared_ptr<domain::interfaces::IUserMetadataProvider> m_provider;
    domain::models::UserMetadata m_data;
};

}
