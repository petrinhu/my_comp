#pragma once
#include "../../domain/interfaces/i_hardware_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class LinuxHardwareProvider : public domain::interfaces::IHardwareProvider {
public:
    explicit LinuxHardwareProvider(std::shared_ptr<IShellExecutor> executor);
    domain::models::HardwareTopology collect_topology() override;

    // Públicos estáticos — pura lógica de parsing, expostos para fuzzing/teste isolado (BT-02).
    static void parse_lsusb_output(const QString& output, std::vector<domain::models::UsbDevice>& devices);
    static void parse_bluetoothctl_output(const QString& output, std::vector<domain::models::BluetoothDevice>& devices);
    // Estado real de um dispositivo a partir de `bluetoothctl info <mac>` (Paired/Connected/Icon).
    static void parse_bluetoothctl_info(const QString& output, domain::models::BluetoothDevice& device);

private:
    std::shared_ptr<IShellExecutor> m_executor;

    void collect_smbios(domain::models::HardwareTopology& topology);
    void collect_usb(domain::models::HardwareTopology& topology);
    void collect_bluetooth(domain::models::HardwareTopology& topology);
};

} // namespace my_comp::infrastructure::collectors
