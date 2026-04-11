#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_audio_provider.h"

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
    
    provider->on_data_ready = [&](const AudioData& data) {
        captured = data;
    };
    
    // Simular recepção de dados
    AudioData fake;
    fake.rms = 0.5f;
    fake.waveform = {0.1f, 0.2f, -0.1f};
    
    provider->on_data_ready(fake);
    
    EXPECT_FLOAT_EQ(captured.rms, 0.5f);
    ASSERT_EQ(captured.waveform.size(), 3);
}
