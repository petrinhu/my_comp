#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

#include <map>

using namespace my_comp::infrastructure::collectors;
using my_comp::domain::models::BluetoothDevice;

namespace {
// Shell falso: responde por "comando args..." e registra as chamadas.
struct ScriptedShell : IShellExecutor {
    std::map<QString, QString> replies;
    QStringList calls;
    int execute(const QString& command, const QStringList& args, QString& output) override {
        const QString key = (QStringList{command} + args).join(' ');
        calls << key;
        auto it = replies.find(key);
        if (it == replies.end()) return 1;
        output = it->second;
        return 0;
    }
};

const char* INFO_HEADSET =
    "Device AA:BB:CC:DD:EE:01 (public)\n"
    "\tName: WH-1000XM4\n"
    "\tAlias: WH-1000XM4\n"
    "\tClass: 0x00240404\n"
    "\tIcon: audio-headset\n"
    "\tPaired: yes\n"
    "\tBonded: yes\n"
    "\tTrusted: yes\n"
    "\tBlocked: no\n"
    "\tConnected: yes\n";

const char* INFO_MOUSE_NEARBY =
    "Device AA:BB:CC:DD:EE:02 (random)\n"
    "\tName: MX Master 3\n"
    "\tIcon: input-mouse\n"
    "\tPaired: no\n"
    "\tConnected: no\n";
} // namespace

// HW-02b: `bluetoothctl devices` lista também dispositivos só conhecidos (não pareados);
// o estado real vem de `bluetoothctl info <mac>`.
TEST(LinuxHardwareBluetoothTest, ParsesInfoForPairedConnectedAndType) {
    BluetoothDevice dev;
    LinuxHardwareProvider::parse_bluetoothctl_info(INFO_HEADSET, dev);
    EXPECT_TRUE(dev.paired);
    EXPECT_TRUE(dev.connected);
    EXPECT_EQ(dev.type, QString("audio-headset"));

    BluetoothDevice mouse;
    LinuxHardwareProvider::parse_bluetoothctl_info(INFO_MOUSE_NEARBY, mouse);
    EXPECT_FALSE(mouse.paired);
    EXPECT_FALSE(mouse.connected);
    EXPECT_EQ(mouse.type, QString("input-mouse"));
}

TEST(LinuxHardwareBluetoothTest, DevicesListDoesNotAssumePaired) {
    std::vector<BluetoothDevice> devs;
    LinuxHardwareProvider::parse_bluetoothctl_output("Device AA:BB:CC:DD:EE:02 MX Master 3\n", devs);
    ASSERT_EQ(devs.size(), 1u);
    EXPECT_EQ(devs[0].name, QString("MX Master 3"));
    EXPECT_FALSE(devs[0].paired);
}

// MAC vai para argv do bluetoothctl: só endereços bem formados passam (MC-10).
TEST(LinuxHardwareBluetoothTest, IgnoresLinesWithoutValidMac) {
    std::vector<BluetoothDevice> devs;
    LinuxHardwareProvider::parse_bluetoothctl_output(
        "Device --help evil\n"
        "Controller AA:BB:CC:DD:EE:FF hci0\n"
        "Device AA:BB:CC:DD:EE:01 Fone\n"
        "Device ZZ:BB:CC:DD:EE:01 Ruim\n", devs);
    ASSERT_EQ(devs.size(), 1u);
    EXPECT_EQ(devs[0].address, QString("AA:BB:CC:DD:EE:01"));
}

TEST(LinuxHardwareBluetoothTest, CollectQueriesInfoPerDevice) {
    auto shell = std::make_shared<ScriptedShell>();
    shell->replies["bluetoothctl devices"] =
        "Device AA:BB:CC:DD:EE:01 WH-1000XM4\nDevice AA:BB:CC:DD:EE:02 MX Master 3\n";
    shell->replies["bluetoothctl info AA:BB:CC:DD:EE:01"] = INFO_HEADSET;
    shell->replies["bluetoothctl info AA:BB:CC:DD:EE:02"] = INFO_MOUSE_NEARBY;

    LinuxHardwareProvider provider(shell);
    const auto topo = provider.collect_topology();
    ASSERT_EQ(topo.bluetoothDevices.size(), 2u);
    EXPECT_TRUE(topo.bluetoothDevices[0].connected);
    EXPECT_EQ(topo.bluetoothDevices[0].type, QString("audio-headset"));
    EXPECT_FALSE(topo.bluetoothDevices[1].paired);
    EXPECT_TRUE(shell->calls.contains("bluetoothctl info AA:BB:CC:DD:EE:02"));
}
