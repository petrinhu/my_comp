#pragma once
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include "../../domain/interfaces/i_hardware_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class HardwareViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap smbios READ smbios NOTIFY dataChanged)
    Q_PROPERTY(QVariantList usbDevices READ usbDevices NOTIFY dataChanged)
    Q_PROPERTY(QVariantList bluetoothDevices READ bluetoothDevices NOTIFY dataChanged)

public:
    explicit HardwareViewModel(std::shared_ptr<domain::interfaces::IHardwareProvider> provider, QObject* parent = nullptr);

    QVariantMap smbios() const;
    QVariantList usbDevices() const;
    QVariantList bluetoothDevices() const;

    Q_INVOKABLE void refresh();

signals:
    void dataChanged();

private:
    std::shared_ptr<domain::interfaces::IHardwareProvider> m_provider;
    domain::models::HardwareTopology m_data;
};

}
