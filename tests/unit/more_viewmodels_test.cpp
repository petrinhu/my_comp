#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/system_info_viewmodel.h"
#include "../../src/presentation/viewmodels/user_metadata_viewmodel.h"
#include "../../src/presentation/viewmodels/hardware_viewmodel.h"
#include "../../src/presentation/viewmodels/service_viewmodel.h"
#include "../../src/presentation/viewmodels/firmware_viewmodel.h"
#include "../../src/domain/interfaces/i_system_info_provider.h"
#include "../../src/domain/interfaces/i_user_metadata_provider.h"
#include "../../src/domain/interfaces/i_hardware_provider.h"
#include "../../src/domain/interfaces/i_service_provider.h"
#include "../../src/domain/interfaces/i_firmware_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

// --- Mocks ---
class MockSystemInfoProvider : public ISystemInfoProvider {
public:
    MOCK_METHOD(SystemInfo, get_system_info, (), (override));
};

class MockUserMetadataProvider : public IUserMetadataProvider {
public:
    MOCK_METHOD(UserMetadata, get_user_metadata, (), (override));
};

class MockHardwareProvider : public IHardwareProvider {
public:
    MOCK_METHOD(HardwareTopology, collect_topology, (), (override));
};

class MockServiceProvider : public IServiceProvider {
public:
    MOCK_METHOD(std::vector<ServiceInfo>, get_services, (), (override));
    MOCK_METHOD(bool, start_service, (const QString&), (override));
    MOCK_METHOD(bool, pause_service, (const QString&), (override));
    MOCK_METHOD(bool, stop_service, (const QString&), (override));
    MOCK_METHOD(OperationResult, create_daemon, (const QString&, const QString&), (override));
};

class MockFirmwareProvider : public IFirmwareProvider {
public:
    MOCK_METHOD(std::vector<FirmwareDevice>, get_devices, (), (override));
    MOCK_METHOD(void, check_updates, (const QString&), (override));
};

// --- Tests ---

TEST(FirmwareViewModelTest, ShouldRefreshData) {
    auto provider = std::make_shared<MockFirmwareProvider>();
    FirmwareDevice dev;
    dev.name = "UEFI";
    dev.has_update = true;
    
    EXPECT_CALL(*provider, get_devices()).WillRepeatedly(Return(std::vector<FirmwareDevice>{dev}));
    
    FirmwareViewModel vm(provider);
    QSignalSpy spy(&vm, &FirmwareViewModel::devicesChanged);
    vm.refresh();
    spy.wait(2000);
    
    EXPECT_FALSE(vm.devices().isEmpty());
    EXPECT_EQ(vm.updateCount(), 1);
}

TEST(SystemInfoViewModelTest, ShouldLoadDataAfterRefresh) {
    auto provider = std::make_shared<MockSystemInfoProvider>();
    SystemInfo info;
    info.distroName = "Fedora";
    info.kernelVersion = "6.12";

    EXPECT_CALL(*provider, get_system_info()).WillRepeatedly(Return(info));

    // Ctor não faz I/O por política (MC-04). Dados só chegam após refresh().
    SystemInfoViewModel vm(provider);
    EXPECT_TRUE(vm.distro().isEmpty());

    vm.refresh();
    EXPECT_TRUE(vm.distro().startsWith("Fedora"));
    EXPECT_EQ(vm.kernel(), "6.12");
}

TEST(UserMetadataViewModelTest, ShouldRefreshData) {
    auto provider = std::make_shared<MockUserMetadataProvider>();
    UserMetadata data;
    data.currentUser = "petrus";
    
    EXPECT_CALL(*provider, get_user_metadata()).WillRepeatedly(Return(data));
    
    UserMetadataViewModel vm(provider);
    vm.refresh();
    EXPECT_EQ(vm.currentUser(), "petrus");
}

TEST(HardwareViewModelTest, ShouldRefreshData) {
    auto provider = std::make_shared<MockHardwareProvider>();
    HardwareTopology data;
    data.smbios.vendor = "Dell";

    EXPECT_CALL(*provider, collect_topology()).WillRepeatedly(Return(data));

    HardwareViewModel vm(provider);
    // AR-08: refresh() dispara QtConcurrent + marshal via QMetaObject::invokeMethod.
    // Aguarda dataChanged via QSignalSpy para drenar: 1ª wait libera quando a task assume,
    // 2ª libera quando o invokeMethod roda no event loop.
    QSignalSpy spy(&vm, &HardwareViewModel::dataChanged);
    vm.initialize();
    ASSERT_TRUE(spy.wait(2000));

    auto smbios = vm.smbios();
    EXPECT_EQ(smbios["vendor"].toString(), "Dell");
}

TEST(ServiceViewModelTest, ShouldRefreshDataAndHandleCommands) {
    auto provider = std::make_shared<MockServiceProvider>();
    ServiceInfo s; s.name = "test.service";
    s.loadState = "loaded";
    s.activeState = "active";
    s.subState = "running";
    s.isEnabled = true;

    EXPECT_CALL(*provider, get_services()).WillRepeatedly(Return(std::vector<ServiceInfo>{s}));
    EXPECT_CALL(*provider, start_service(_)).WillRepeatedly(Return(true));

    ServiceViewModel vm(provider);
    QSignalSpy spy(&vm, &ServiceViewModel::servicesChanged);
    
    vm.refresh();
    spy.wait(2000); // Aguarda até 2s pela thread
    
    auto list = vm.services();
    EXPECT_FALSE(list.isEmpty());

    vm.startService("test.service");
    // startService também é assíncrono e chama refresh() no sucesso
    spy.wait(2000);
}

// SV-03c: a mensagem concreta do provider (ex.: "não é executável") chega à UI,
// em vez do genérico "Falha ao criar daemon.".
TEST(ServiceViewModelTest, CreateDaemonForwardsProviderMessage) {
    auto provider = std::make_shared<NiceMock<MockServiceProvider>>();
    ON_CALL(*provider, get_services()).WillByDefault(Return(std::vector<ServiceInfo>{}));
    EXPECT_CALL(*provider, create_daemon(QString("/tmp/x"), QString("")))
        .WillOnce(Return(OperationResult{.ok = false, .message = "O arquivo /tmp/x não é executável."}));

    ServiceViewModel vm(provider);
    QSignalSpy finished(&vm, &ServiceViewModel::operationFinished);
    vm.createDaemon("/tmp/x", "");
    ASSERT_TRUE(finished.wait(2000));
    EXPECT_FALSE(finished.at(0).at(0).toBool());
    EXPECT_EQ(finished.at(0).at(1).toString(), QString("O arquivo /tmp/x não é executável."));
}
