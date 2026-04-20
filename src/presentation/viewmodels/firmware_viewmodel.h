#pragma once
#include <QObject>
#include <QVariantList>
#include <memory>
#include "../../domain/interfaces/i_firmware_provider.h"

namespace my_comp::presentation::viewmodels {

class FirmwareViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(int updateCount READ updateCount NOTIFY devicesChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit FirmwareViewModel(std::shared_ptr<domain::interfaces::IFirmwareProvider> provider, QObject *parent = nullptr);

    QVariantList devices() const;
    int updateCount() const { return m_update_count; }
    bool isBusy() const { return m_is_busy; }

public slots:
    void refresh();

signals:
    void devicesChanged();
    void isBusyChanged();

private:
    std::shared_ptr<domain::interfaces::IFirmwareProvider> m_provider;
    std::vector<domain::models::FirmwareDevice> m_devices;
    int m_update_count = 0;
    bool m_is_busy = false;
};

} // namespace my_comp::presentation::viewmodels
