#include "qt_audio_provider.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <cmath>
#include <algorithm>
#include <complex>
#include <vector>
#include <bit>

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
    
    // Gerar espectro de frequência via FFT
    audioData.spectrum = calculate_fft(samples, count);
    
    if (on_data_ready) {
        on_data_ready(audioData);
    }
}

std::vector<float> QtAudioProvider::calculate_fft(const float* samples, int count) {
    if (count < 2) return {};

    // Forçar potência de 2 para o algoritmo Radix-2
    size_t n = std::bit_floor(static_cast<size_t>(count));
    std::vector<std::complex<double>> data(n);
    
    // Aplicar Janela de Hann para reduzir spectral leakage
    for (size_t i = 0; i < n; ++i) {
        double multiplier = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (n - 1)));
        data[i] = static_cast<double>(samples[i]) * multiplier;
    }

    // Bit-reversal permutation
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }

    // Cooley-Tukey iterative
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

    // Extrair magnitudes da metade do espectro (Nyquist)
    // Usar normalização por 2/n para compensar a perda da janela e normalizar magnitude
    std::vector<float> spectrum(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        spectrum[i] = static_cast<float>(std::abs(data[i]) * 2.0 / n);
    }
    
    return spectrum;
}

} // namespace my_comp::infrastructure::audio
