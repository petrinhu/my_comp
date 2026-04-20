#pragma once
#include "../models/audio_data.h"
#include <functional>
#include <mutex>

namespace my_comp::domain::interfaces {

class IAudioProvider {
public:
    virtual ~IAudioProvider() = default;

    virtual bool start_capture() = 0;
    virtual void stop_capture() = 0;
    virtual bool is_capturing() const = 0;

    // Callback encapsulado. Implementações concretas são responsáveis por
    // marshalar a chamada para a thread do consumidor (ex.: via
    // QMetaObject::invokeMethod) antes de chamar notify_data_ready().
    void set_data_callback(std::function<void(const models::AudioData&)> cb) {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        callback_ = std::move(cb);
    }

    void notify_data_ready(const models::AudioData& data) {
        std::function<void(const models::AudioData&)> cb;
        {
            std::lock_guard<std::mutex> lock(callback_mutex_);
            cb = callback_;
        }
        if (cb) cb(data);
    }

private:
    std::function<void(const models::AudioData&)> callback_;
    std::mutex callback_mutex_;
};

} // namespace my_comp::domain::interfaces
