// QA-03 — Aceitação: aba Hardware pela pilha real (HardwareViewModel → LinuxHardwareProvider),
// com lsusb/bluetoothctl roteirizados e um diretório DMI de fixture.
#include <gtest/gtest.h>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest/QSignalSpy>

#include "scripted_shell.h"
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"
#include "../../src/presentation/viewmodels/hardware_viewmodel.h"

using my_comp::infrastructure::collectors::LinuxHardwareProvider;
using my_comp::presentation::viewmodels::HardwareViewModel;
using my_comp::tests::ScriptedShell;

namespace {
void write(const QString& dir, const QString& name, const QByteArray& v) {
    QFile f(dir + "/" + name);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(v);
}
} // namespace

TEST(HardwareTopologyAcceptance, UserSeesSmbiosUsbAndBluetoothState) {
    QTemporaryDir dmi;
    ASSERT_TRUE(dmi.isValid());
    write(dmi.path(), "sys_vendor", "LENOVO\n");
    write(dmi.path(), "product_name", "21HM\n");
    write(dmi.path(), "product_version", "To Be Filled By O.E.M.\n");

    auto shell = std::make_shared<ScriptedShell>();
    shell->reply("lsusb", "Bus 001 Device 003: ID 046d:c52b Logitech, Inc. Unifying Receiver\n");
    shell->reply("bluetoothctl devices", "Device AA:BB:CC:DD:EE:01 WH-1000XM4\n");
    shell->reply("bluetoothctl info AA:BB:CC:DD:EE:01", "\tIcon: audio-headset\n\tPaired: yes\n\tConnected: yes\n");

    HardwareViewModel vm(std::make_shared<LinuxHardwareProvider>(shell, dmi.path()));
    QSignalSpy changed(&vm, &HardwareViewModel::dataChanged);
    vm.initialize();
    ASSERT_TRUE(changed.wait(5000));

    EXPECT_EQ(vm.smbios()["vendor"].toString(), QString("LENOVO"));
    EXPECT_TRUE(vm.smbios()["version"].toString().isEmpty()); // placeholder O.E.M. → "—" na UI
    ASSERT_EQ(vm.usbDevices().size(), 1);
    EXPECT_EQ(vm.usbDevices().at(0).toMap()["vendorId"].toString(), QString("046d"));
    ASSERT_EQ(vm.bluetoothDevices().size(), 1);
    EXPECT_TRUE(vm.bluetoothDevices().at(0).toMap()["connected"].toBool());
}
