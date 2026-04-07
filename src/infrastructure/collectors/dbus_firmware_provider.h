#pragma once
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include "../../domain/interfaces/i_firmware_provider.h"

namespace my_comp::infrastructure::collectors {

class DBusFirmwareProvider : public domain::interfaces::IFirmwareProvider {
public:
    DBusFirmwareProvider();
    
    std::vector<domain::models::FirmwareDevice> get_devices() override;
    void check_updates(const QString& device_id) override;

private:
    std::unique_ptr<QDBusInterface> m_interface;
};

} // namespace my_comp::infrastructure::collectors
