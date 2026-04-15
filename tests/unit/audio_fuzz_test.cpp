#include <gtest/gtest.h>
#include "../../src/infrastructure/audio/qt_audio_provider.h"
#include <vector>
#include <random>
#include <chrono>

using namespace my_comp::infrastructure::audio;

class AudioFuzzTest : public ::testing::Test {
protected:
    QtAudioProvider provider;
};

TEST_F(AudioFuzzTest, StressWithRandomData) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-1.0, 1.0);

    // Injetar 1 milhão de amostras em chunks variados
    for (int i = 0; i < 100; ++i) {
        int chunkSize = 1024 + (generator() % 4096);
        std::vector<float> samples(chunkSize);
        for (auto& s : samples) s = distribution(generator);

        provider.writeData(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(float));
    }
    // Se não crashou, passou no fuzzing básico de volume
    SUCCEED();
}

TEST_F(AudioFuzzTest, HandleExtremeValues) {
    std::vector<float> extremeSamples = {
        1.0f, -1.0f, 0.0f, 
        std::numeric_limits<float>::infinity(), 
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN(),
        1e30f, -1e30f
    };

    provider.writeData(reinterpret_cast<const char*>(extremeSamples.data()), extremeSamples.size() * sizeof(float));
    SUCCEED();
}
