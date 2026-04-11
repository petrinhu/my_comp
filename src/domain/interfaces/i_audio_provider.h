#pragma once
#include "../models/audio_data.h"
#include <functional>

namespace my_comp::domain::interfaces {

class IAudioProvider {
public:
    virtual ~IAudioProvider() = default;

    virtual bool start_capture() = 0;
    virtual void stop_capture() = 0;
    virtual bool is_capturing() const = 0;

    // Callback para dados em tempo real
    std::function<void(const models::AudioData&)> on_data_ready;
};

} // namespace my_comp::domain::interfaces
