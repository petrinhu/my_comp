#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <memory>
#include <cmath>
#include "../../src/domain/interfaces/i_audio_provider.h"
#include "../../src/infrastructure/audio/qt_audio_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockAudioProvider : public IAudioProvider {
public:
    MOCK_METHOD(bool, start_capture, (), (override));
    MOCK_METHOD(void, stop_capture, (), (override));
    MOCK_METHOD(bool, is_capturing, (), (const, override));
};

TEST(AudioCaptureAcceptance, ShouldManageCaptureLifecycle) {
    auto provider = std::make_unique<MockAudioProvider>();
    
    EXPECT_CALL(*provider, start_capture()).WillOnce(Return(true));
    EXPECT_CALL(*provider, is_capturing()).WillRepeatedly(Return(true));
    EXPECT_CALL(*provider, stop_capture()).Times(1);
    
    EXPECT_TRUE(provider->start_capture());
    EXPECT_TRUE(provider->is_capturing());
    provider->stop_capture();
}

TEST(AudioCaptureAcceptance, ShouldReceiveDataViaCallback) {
    auto provider = std::make_unique<MockAudioProvider>();
    AudioData captured;

    provider->set_data_callback([&](const AudioData& data) {
        captured = data;
    });

    // Simular recepção de dados — Mock não marshalha; notify_data_ready é síncrono.
    AudioData fake;
    fake.rms = 0.5f;
    fake.waveform = {0.1f, 0.2f, -0.1f, 0.0f, 0.1f, 0.2f, -0.1f, 0.0f};
    fake.spectrum = {0.1f, 0.5f, 0.1f};

    provider->notify_data_ready(fake);

    EXPECT_FLOAT_EQ(captured.rms, 0.5f);
    ASSERT_EQ(captured.waveform.size(), 8);
    EXPECT_FALSE(captured.spectrum.empty());
}

TEST(AudioCaptureAcceptance, ShouldGenerateFFTSpectrum) {
    using namespace my_comp::infrastructure::audio;
    // QtAudioProvider marshalha notify_data_ready via QueuedConnection — precisa de event loop.
    int argc = 1;
    char* argv[] = {(char*)"test"};
    QCoreApplication app(argc, argv);

    QtAudioProvider provider;
    AudioData captured;

    provider.set_data_callback([&](const AudioData& data) {
        captured = data;
    });

    // Simular 1024 amostras (potência de 2 é ideal para FFT)
    std::vector<float> samples(1024);
    for (int i = 0; i < 1024; ++i) {
        samples[i] = std::sin(2.0 * M_PI * 440.0 * i / 44100.0); // Tom de 440Hz
    }

    provider.writeData(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(float));
    QCoreApplication::processEvents(); // drenar notify_data_ready marshalado

    EXPECT_FALSE(captured.spectrum.empty());
}
