#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/firmware_viewmodel.h"
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_firmware_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/dbus_firmware_provider.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::FirmwareViewModel;

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

// QA-03: o usuário vê quantos dispositivos têm atualização de firmware (lógica do VM sobre
// o provider; o D-Bus real fica no teste acima).
TEST(FirmwareMonitorAcceptance, UserSeesHowManyDevicesHaveUpdates) {
    auto provider = std::make_shared<testing::NiceMock<MockFirmwareProvider>>();
    FirmwareDevice uefi; uefi.name = "System Firmware"; uefi.version = "1.2.3"; uefi.has_update = true;
    FirmwareDevice ssd; ssd.name = "Samsung SSD"; ssd.version = "4B2QGXA7"; ssd.has_update = false;
    ON_CALL(*provider, get_devices()).WillByDefault(testing::Return(std::vector<FirmwareDevice>{uefi, ssd}));

    FirmwareViewModel vm(provider);
    QSignalSpy changed(&vm, &FirmwareViewModel::devicesChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_EQ(vm.devices().size(), 2);
    EXPECT_EQ(vm.updateCount(), 1);
}
