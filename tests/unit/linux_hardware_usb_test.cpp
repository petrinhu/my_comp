#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::domain::models::UsbDevice;

// HW-02c: snapshots reais de `lsusb` (usbutils 015/017) como regressão do formato.
TEST(LinuxHardwareUsbTest, ParsesRealLsusbSnapshot) {
    const QString snapshot =
        "Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub\n"
        "Bus 001 Device 003: ID 046d:c52b Logitech, Inc. Unifying Receiver\n"
        "Bus 002 Device 004: ID 0bda:8153 Realtek Semiconductor Corp. RTL8153 Gigabit Ethernet Adapter\n"
        "Bus 003 Device 002: ID 8087:0033 Intel Corp. AX211 Bluetooth\n";
    std::vector<UsbDevice> devs;
    LinuxHardwareProvider::parse_lsusb_output(snapshot, devs);
    ASSERT_EQ(devs.size(), 4u);
    EXPECT_EQ(devs[1].bus, QString("001"));
    EXPECT_EQ(devs[1].vendorId, QString("046d"));
    EXPECT_EQ(devs[1].productId, QString("c52b"));
    EXPECT_EQ(devs[1].name, QString("Logitech, Inc. Unifying Receiver"));
    EXPECT_EQ(devs[3].name, QString("Intel Corp. AX211 Bluetooth"));
}

// Dispositivo sem descrição no usb.ids: a linha termina logo após o ID.
TEST(LinuxHardwareUsbTest, AcceptsDeviceWithoutName) {
    std::vector<UsbDevice> devs;
    LinuxHardwareProvider::parse_lsusb_output("Bus 004 Device 007: ID 1234:abcd\n", devs);
    ASSERT_EQ(devs.size(), 1u);
    EXPECT_EQ(devs[0].vendorId, QString("1234"));
    EXPECT_TRUE(devs[0].name.isEmpty());
}

// Espaços extras não podem deslocar colunas (o split por ' ' quebrava aqui).
TEST(LinuxHardwareUsbTest, ToleratesExtraWhitespace) {
    std::vector<UsbDevice> devs;
    LinuxHardwareProvider::parse_lsusb_output("Bus  001 Device 003:  ID 046d:c52b   Logitech   Receiver  \n", devs);
    ASSERT_EQ(devs.size(), 1u);
    EXPECT_EQ(devs[0].productId, QString("c52b"));
    EXPECT_EQ(devs[0].name, QString("Logitech   Receiver"));
}

// Formato inesperado é ignorado em vez de virar dispositivo com campos trocados.
TEST(LinuxHardwareUsbTest, IgnoresLinesOutsideTheFormat) {
    std::vector<UsbDevice> devs;
    LinuxHardwareProvider::parse_lsusb_output(
        "unable to initialize libusb: -99\n"
        "Bus 001 Device 002: ID zzzz:0001 Inválido\n"
        "/:  Bus 01.Port 1: Dev 1, Class=root_hub, Driver=xhci_hcd/12p, 480M\n", devs);
    EXPECT_TRUE(devs.empty());
}
