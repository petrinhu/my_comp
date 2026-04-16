#include "qt_audio_provider.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <complex>
#include <vector>
#include <bit>

namespace my_comp::infrastructure::audio {

// --- AudioSink Implementation ---

AudioSink::AudioSink(std::function<void(const char*, qint64)> on_data, QObject* parent)
    : QIODevice(parent), m_callback(std::move(on_data)) {
    open(QIODevice::WriteOnly);
}

qint64 AudioSink::writeData(const char* data, qint64 len) {
    if (m_callback) m_callback(data, len);
    return len;
}

// --- QtAudioProvider Implementation ---

QtAudioProvider::QtAudioProvider(QObject* parent) : QObject(parent) {
    m_format.setSampleRate(44100);
    m_format.setChannelCount(1);
    m_format.setSampleFormat(QAudioFormat::Float);

    auto device = QMediaDevices::defaultAudioInput();
    if (!device.isNull()) {
        m_audioSource = std::make_unique<QAudioSource>(device, m_format);
    } else {
        qWarning() << "[Audio] No default audio input device found!";
    }

    // Criar o sink que servirá como alvo para o áudio
    m_sink = std::make_unique<AudioSink>([this](const char* data, qint64 len) {
        process_raw_data(data, len);
    }, this);
}

QtAudioProvider::~QtAudioProvider() {
    stop_capture();
}

bool QtAudioProvider::start_capture() {
    if (m_capturing) return true;
    if (!m_audioSource || !m_sink) return false;
    
    m_audioSource->start(m_sink.get());
    m_capturing = true;
    return true;
}

void QtAudioProvider::stop_capture() {
    if (!m_capturing) return;
    if (m_audioSource) m_audioSource->stop();
    m_capturing = false;
}

void QtAudioProvider::writeData(const char* data, qint64 len) {
    process_raw_data(data, len);
}

void QtAudioProvider::process_raw_data(const char* data, qint64 len) {
    int sampleCount = static_cast<int>(len / sizeof(float));
    const float* samples = reinterpret_cast<const float*>(data);
    
    if (sampleCount > 0) {
        process_samples(samples, sampleCount);
    }
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
    audioData.spectrum = calculate_fft(samples, count);
    
    if (on_data_ready) {
        on_data_ready(audioData);
    }
}

std::vector<float> QtAudioProvider::calculate_fft(const float* samples, int count) {
    if (count < 2) return {};
    size_t n = std::bit_floor(static_cast<size_t>(count));
    std::vector<std::complex<double>> data(n);
    
    for (size_t i = 0; i < n; ++i) {
        double multiplier = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (n - 1)));
        data[i] = static_cast<double>(samples[i]) * multiplier;
    }

    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(data[i], data[j]);
    }

    for (size_t len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * M_PI / len;
        std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (size_t i = 0; i < n; i += len) {
            std::complex<double> w(1);
            for (size_t j = 0; j < len / 2; ++j) {
                std::complex<double> u = data[i + j];
                std::complex<double> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    std::vector<float> spectrum(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        spectrum[i] = static_cast<float>(std::abs(data[i]) * 2.0 / n);
    }
    return spectrum;
}

} // namespace my_comp::infrastructure::audio
