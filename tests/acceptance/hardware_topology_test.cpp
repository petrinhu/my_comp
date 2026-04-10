#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_hardware_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockHardwareProvider : public IHardwareProvider {
public:
    MOCK_METHOD(HardwareTopology, collect_topology, (), (override));
};

TEST(HardwareTopologyAcceptance, ShouldCollectHardwareDataContract) {
    auto provider = std::make_unique<MockHardwareProvider>();
    
    HardwareTopology fake_data;
    fake_data.smbios.vendor = "Acer";
    fake_data.smbios.model = "Nitro 5";
    
    UsbDevice usb;
    usb.name = "USB Optical Mouse";
    fake_data.usbDevices.push_back(usb);
    
    BluetoothDevice bt;
    bt.name = "Sony WH-1000XM4";
    bt.paired = true;
    fake_data.bluetoothDevices.push_back(bt);
    
    EXPECT_CALL(*provider, collect_topology()).WillOnce(testing::Return(fake_data));
    
    auto data = provider->collect_topology();
    EXPECT_EQ(data.smbios.vendor, "Acer");
    ASSERT_EQ(data.usbDevices.size(), 1);
    EXPECT_EQ(data.usbDevices[0].name, "USB Optical Mouse");
    ASSERT_EQ(data.bluetoothDevices.size(), 1);
    EXPECT_TRUE(data.bluetoothDevices[0].paired);
}
