// BT-02 — libFuzzer harness combinado para parse_lsusb_output e parse_bluetoothctl_output.
// Alterna entre os dois alvos pelo 1º byte do input, garantindo cobertura igual.

#include <cstdint>
#include <cstddef>
#include <vector>
#include <QString>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"

using my_comp::infrastructure::collectors::LinuxHardwareProvider;
using my_comp::domain::models::UsbDevice;
using my_comp::domain::models::BluetoothDevice;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0 || size > 65536) return 0;
    const QString input = QString::fromUtf8(reinterpret_cast<const char*>(data + 1), static_cast<int>(size - 1));
    if (data[0] & 1) {
        std::vector<UsbDevice> out;
        LinuxHardwareProvider::parse_lsusb_output(input, out);
    } else {
        std::vector<BluetoothDevice> out;
        LinuxHardwareProvider::parse_bluetoothctl_output(input, out);
    }
    return 0;
}
