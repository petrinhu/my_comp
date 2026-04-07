#pragma once
#include <vector>
#include "../models/firmware_device.h"

namespace my_comp::domain::interfaces {

class IFirmwareProvider {
public:
    virtual ~IFirmwareProvider() = default;

    /**
     * @brief Lista todos os dispositivos de firmware via LVFS/fwupd.
     */
    virtual std::vector<models::FirmwareDevice> get_devices() = 0;

    /**
     * @brief Verifica se há atualizações para um dispositivo específico.
     */
    virtual void check_updates(const QString& device_id) = 0;
};

} // namespace my_comp::domain::interfaces
