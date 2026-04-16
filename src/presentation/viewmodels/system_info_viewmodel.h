#pragma once
#include <QObject>
#include "../../domain/interfaces/i_system_info_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class SystemInfoViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString distro READ distro NOTIFY dataChanged)
    Q_PROPERTY(QString kernel READ kernel NOTIFY dataChanged)
    Q_PROPERTY(QString driver READ driver NOTIFY dataChanged)
public:
    explicit SystemInfoViewModel(std::shared_ptr<domain::interfaces::ISystemInfoProvider> provider, QObject* parent = nullptr);

    QString distro() const { return m_distro; }
    QString kernel() const { return m_kernel; }
    QString driver() const { return m_driver; }

    Q_INVOKABLE void refresh();

signals:
    void dataChanged();

private:
    std::shared_ptr<domain::interfaces::ISystemInfoProvider> m_provider;
    QString m_distro;
    QString m_kernel;
    QString m_driver;
};

}
