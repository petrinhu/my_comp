#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_firmware_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/dbus_firmware_provider.h"

using namespace my_comp::infrastructure::collectors;

TEST(FirmwareMonitorAcceptance, ShouldCollectFromRealSystem) {
    DBusFirmwareProvider provider;
    auto result = provider.get_devices();
    
    // Na maioria dos sistemas modernos (Fedora 43), deve haver ao menos um dispositivo
    // (ex: UEFI, SSD, etc)
    std::cout << "[Firmware Test] Found " << result.size() << " devices" << std::endl;
    for (const auto& dev : result) {
        std::cout << "  - " << dev.name.toStdString() << " (" << dev.version.toStdString() << ")" << std::endl;
    }
}

class MockFirmwareProvider : public IFirmwareProvider {
public:
    MOCK_METHOD(std::vector<FirmwareDevice>, get_devices, (), (override));
    MOCK_METHOD(void, check_updates, (const QString&), (override));
};

TEST(FirmwareMonitorAcceptance, ShouldCollectFirmwareDevices) {
    auto provider = std::make_unique<MockFirmwareProvider>();
    
    FirmwareDevice dev;
    dev.name = "System Firmware";
    dev.version = "1.2.3";
    
    std::vector<FirmwareDevice> devs = {dev};
    
    EXPECT_CALL(*provider, get_devices()).WillOnce(testing::Return(devs));
    
    auto result = provider->get_devices();
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].name, "System Firmware");
}
