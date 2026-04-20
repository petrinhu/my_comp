#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/process_viewmodel.h"
#include "../../src/presentation/viewmodels/dev_stack_viewmodel.h"
#include "../../src/presentation/viewmodels/export_viewmodel.h"
#include "../../src/presentation/viewmodels/audio_viewmodel.h"
#include "../../src/presentation/viewmodels/installation_viewmodel.h"
#include "../../src/domain/interfaces/i_process_provider.h"
#include "../../src/domain/interfaces/i_dev_stack_provider.h"
#include "../../src/domain/interfaces/i_audio_provider.h"
#include "../../src/domain/interfaces/i_installer.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

// --- Mocks ---
class MockProcessProvider : public IProcessProvider {
public:
    MOCK_METHOD(std::vector<ProcessInfo>, collect_processes, (), (override));
    MOCK_METHOD(bool, kill_process, (int, int), (override));
    MOCK_METHOD(bool, set_priority, (int, int), (override));
};

class MockDevStackProvider : public IDevStackProvider {
public:
    MOCK_METHOD(DevStackInfo, collect_dev_stacks, (), (override));
};

class MockAudioProvider : public IAudioProvider {
public:
    MOCK_METHOD(bool, start_capture, (), (override));
    MOCK_METHOD(void, stop_capture, (), (override));
    MOCK_METHOD(bool, is_capturing, (), (const, override));
};

class MockInstaller : public IInstaller {
public:
    MOCK_METHOD(void, install, (const my_comp::domain::models::UpdatePackage&), (override));
    MOCK_METHOD(void, upgradeAll, (), (override));
    MOCK_METHOD(void, sendResponse, (const QString&), (override));
    void set_output_handler(std::function<void(const QString&)>) override {}
    void set_interaction_handler(std::function<void(const QString&)>) override {}
    void set_finished_handler(std::function<void(int)>) override {}
};

// --- Tests ---

TEST(ProcessViewModelTest, ShouldFilterAndSort) {
    auto provider = std::make_shared<MockProcessProvider>();
    ProcessInfo p1; p1.pid = 1; p1.command = "bash"; p1.cpuPercent = 10.0;
    ProcessInfo p2; p2.pid = 2; p2.command = "top"; p2.cpuPercent = 5.0;
    
    // Forçar WillRepeatedly para garantir que sempre tenha dados durante o ciclo do timer/refresh
    EXPECT_CALL(*provider, collect_processes()).WillRepeatedly(Return(std::vector<ProcessInfo>{p1, p2}));
    
    ProcessViewModel vm(provider);
    vm.refresh();
    
    auto list = vm.processes();
    EXPECT_GT(list.size(), 0);
    
    vm.setFilter(QStringLiteral("top"));
    EXPECT_EQ(vm.processes().size(), 1);
}

TEST(DevStackViewModelTest, ShouldRefresh) {
    auto provider = std::make_shared<MockDevStackProvider>();
    DevStackInfo info; info.toolchains.push_back({"gcc", "14.0"});
    
    EXPECT_CALL(*provider, collect_dev_stacks()).WillRepeatedly(Return(info));
    
    DevStackViewModel vm(provider);
    QSignalSpy spy(&vm, &DevStackViewModel::dataChanged);
    vm.refresh();
    spy.wait(2000);
    
    EXPECT_GT(vm.toolchains().size(), 0);
}

TEST(AudioViewModelTest, ShouldControlCapture) {
    auto provider = std::make_shared<MockAudioProvider>();
    EXPECT_CALL(*provider, start_capture()).WillRepeatedly(Return(true));
    EXPECT_CALL(*provider, stop_capture()).Times(AtLeast(1));
    
    AudioViewModel vm(provider);
    vm.start();
    vm.stop();
}

TEST(InstallationViewModelTest, ShouldForwardCommands) {
    auto installer = std::make_shared<MockInstaller>();
    // InstallationViewModel::startInstallation mapeia int para UpdatePackage e chama installer->install()
    EXPECT_CALL(*installer, install(_)).Times(AtLeast(1));
    EXPECT_CALL(*installer, sendResponse(QStringLiteral("y"))).Times(AtLeast(1));
    
    InstallationViewModel vm(installer);
    vm.startInstallation(QStringLiteral("git"), (int)ProviderType::Dnf);
    vm.respond(true);
}
