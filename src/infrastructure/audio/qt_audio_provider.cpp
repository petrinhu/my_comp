#include "qt_audio_provider.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <cmath>
#include <algorithm>

namespace my_comp::infrastructure::audio {

QtAudioProvider::QtAudioProvider(QObject* parent) : QIODevice(parent) {
    m_format.setSampleRate(44100);
    m_format.setChannelCount(1);
    m_format.setSampleFormat(QAudioFormat::Float);

    auto device = QMediaDevices::defaultAudioInput();
    m_audioSource = std::make_unique<QAudioSource>(device, m_format);
}

QtAudioProvider::~QtAudioProvider() {
    stop_capture_impl();
}

bool QtAudioProvider::start_capture() {
    if (m_capturing) return true;
    
    if (open(QIODevice::WriteOnly)) {
        m_audioSource->start(this);
        m_capturing = true;
        return true;
    }
    return false;
}

void QtAudioProvider::stop_capture() {
    stop_capture_impl();
}

void QtAudioProvider::stop_capture_impl() {
    if (!m_capturing) return;
    m_audioSource->stop();
    close();
    m_capturing = false;
}

qint64 QtAudioProvider::writeData(const char *data, qint64 len) {
    int sampleCount = static_cast<int>(len / sizeof(float));
    const float* samples = reinterpret_cast<const float*>(data);
    
    if (sampleCount > 0) {
        process_samples(samples, sampleCount);
    }
    
    return len;
}

void QtAudioProvider::process_samples(const float* samples, int count) {
    domain::models::AudioData audioData;
    audioData.waveform.assign(samples, samples + count);
    
    float sumSq = 0;
    float peak = 0;
    
    for (int i = 0; i < count; ++i) {
        float s = samples[i];
        sumSq += s * s;
        peak = std::max(peak, std::abs(s));
    }
    
    audioData.rms = std::sqrt(sumSq / count);
    audioData.peak = peak;
    
    // Simplificação: FFT (Espectro) será implementado em próxima iteração se necessário
    // Por enquanto focado na Waveform real-time exigida.
    
    if (on_data_ready) {
        on_data_ready(audioData);
    }
}

} // namespace my_comp::infrastructure::audio
