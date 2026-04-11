#pragma once
#include <vector>
#include <QString>

namespace my_comp::domain::models {

struct AudioData {
    std::vector<float> waveform;   // Amostras de amplitude no tempo
    std::vector<float> spectrum;   // Magnitudes de frequência (FFT)
    float rms;                     // Volume médio (Root Mean Square)
    float peak;                    // Volume de pico
};

} // namespace my_comp::domain::models
