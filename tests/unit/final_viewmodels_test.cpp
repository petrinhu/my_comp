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

    EXPECT_CALL(*provider, collect_processes()).WillRepeatedly(Return(std::vector<ProcessInfo>{p1, p2}));

    ProcessViewModel vm(provider);

    // refresh() roda em QtConcurrent + QueuedConnection — aguardar via signal
    QSignalSpy spy(&vm, &ProcessViewModel::processesChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));

    auto list = vm.processes();
    EXPECT_GT(list.size(), 0);

    // setFilter agora rebuilda a lista a partir do m_data cacheado (sem novo refresh)
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

TEST(AudioViewModelTest, ShouldProduceBandCountBinsAfterTransform) {
    auto provider = std::make_shared<MockAudioProvider>();
    EXPECT_CALL(*provider, stop_capture()).Times(AtLeast(0));
    AudioViewModel vm(provider);

    vm.setSpectrumBandCount(32);
    vm.setSpectrumAttack(1.0f);
    vm.setSpectrumRelease(1.0f);

    std::vector<float> fft(512, 0.5f);
    auto out = vm.transformSpectrum(fft, 44100.0f);
    EXPECT_EQ(out.size(), 32u);
}

TEST(AudioViewModelTest, ShouldSmoothSpectrumAsymmetrically) {
    auto provider = std::make_shared<MockAudioProvider>();
    EXPECT_CALL(*provider, stop_capture()).Times(AtLeast(0));
    AudioViewModel vm(provider);

    vm.setSpectrumBandCount(16);
    vm.setSpectrumAttack(1.0f);
    vm.setSpectrumRelease(0.2f);

    std::vector<float> hot(512, 1.0f);
    auto peak = vm.transformSpectrum(hot, 44100.0f);
    ASSERT_EQ(peak.size(), 16u);
    for (float v : peak) EXPECT_NEAR(v, 1.0f, 1e-5f);

    std::vector<float> cold(512, 0.0f);
    auto decayed = vm.transformSpectrum(cold, 44100.0f);
    for (float v : decayed) {
        EXPECT_LT(v, 1.0f);
        EXPECT_GT(v, 0.0f);
    }

    auto decayed2 = vm.transformSpectrum(cold, 44100.0f);
    for (size_t i = 0; i < decayed.size(); ++i) EXPECT_LT(decayed2[i], decayed[i]);
}

TEST(AudioViewModelTest, ShouldDistributeBinsLogarithmically) {
    auto provider = std::make_shared<MockAudioProvider>();
    EXPECT_CALL(*provider, stop_capture()).Times(AtLeast(0));
    AudioViewModel vm(provider);

    vm.setSpectrumBandCount(8);
    vm.setSpectrumMinHz(30.0f);
    vm.setSpectrumMaxHz(20000.0f);
    vm.setSpectrumAttack(1.0f);
    vm.setSpectrumRelease(1.0f);

    // FFT de 1024 bins: bin i cobre i * (44100 / 2048) Hz ≈ 21.5 Hz por bin.
    // Só o bin 1 (~21 Hz) tem energia, todas bandas > 30 Hz devem ficar perto de zero.
    std::vector<float> fft(1024, 0.0f);
    fft[1] = 1.0f;
    auto low = vm.transformSpectrum(fft, 44100.0f);
    ASSERT_EQ(low.size(), 8u);

    // Fogo em bin correspondente a ~10 kHz (bin ≈ 10000/21.5 ≈ 464).
    std::fill(fft.begin(), fft.end(), 0.0f);
    fft[464] = 1.0f;
    vm.setSpectrumAttack(1.0f);
    vm.setSpectrumRelease(1.0f); // reset EMA para igual → instantâneo
    auto high = vm.transformSpectrum(fft, 44100.0f);

    // Banda final (cobre até 20 kHz) deve captar os 10 kHz; banda inicial não.
    EXPECT_GT(high[7], high[0]);
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
