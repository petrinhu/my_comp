#include "audio_viewmodel.h"
#include <algorithm>
#include <cmath>

namespace my_comp::presentation::viewmodels {

namespace {
constexpr float kSampleRateHz = 44100.0f;
}

AudioViewModel::AudioViewModel(std::shared_ptr<domain::interfaces::IAudioProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {

    reset_smoothing();
    if (!m_provider) return;

    m_provider->set_data_callback([this](const domain::models::AudioData& data) {
        update_data(data);
    });
}

void AudioViewModel::reset_smoothing() {
    m_smoothed.assign(static_cast<size_t>(std::max(1, m_band_count)), 0.0f);
}

void AudioViewModel::setSpectrumBandCount(int n) {
    n = std::clamp(n, 4, 512);
    if (n == m_band_count) return;
    m_band_count = n;
    reset_smoothing();
    emit spectrumConfigChanged();
}

void AudioViewModel::setSpectrumMinHz(float hz) {
    hz = std::max(1.0f, hz);
    if (hz == m_min_hz) return;
    m_min_hz = hz;
    emit spectrumConfigChanged();
}

void AudioViewModel::setSpectrumMaxHz(float hz) {
    hz = std::max(m_min_hz + 1.0f, hz);
    if (hz == m_max_hz) return;
    m_max_hz = hz;
    emit spectrumConfigChanged();
}

void AudioViewModel::setSpectrumAttack(float a) {
    a = std::clamp(a, 0.0f, 1.0f);
    if (a == m_attack) return;
    m_attack = a;
    emit spectrumConfigChanged();
}

void AudioViewModel::setSpectrumRelease(float r) {
    r = std::clamp(r, 0.0f, 1.0f);
    if (r == m_release) return;
    m_release = r;
    emit spectrumConfigChanged();
}

std::vector<float> AudioViewModel::transformSpectrum(const std::vector<float>& fft_bins, float sample_rate_hz) {
    const size_t band_count = static_cast<size_t>(std::max(1, m_band_count));
    if (m_smoothed.size() != band_count) m_smoothed.assign(band_count, 0.0f);
    if (fft_bins.empty() || sample_rate_hz <= 0.0f) {
        return m_smoothed;
    }

    const size_t n_fft = fft_bins.size() * 2;
    const float bin_hz = sample_rate_hz / static_cast<float>(n_fft);
    const float max_hz = std::min(m_max_hz, sample_rate_hz * 0.5f);
    const float min_hz = std::min(m_min_hz, max_hz - 1.0f);
    const float log_min = std::log(min_hz);
    const float log_max = std::log(max_hz);
    const float log_step = (log_max - log_min) / static_cast<float>(band_count);

    std::vector<float> raw(band_count, 0.0f);
    for (size_t b = 0; b < band_count; ++b) {
        const float lo_hz = std::exp(log_min + log_step * static_cast<float>(b));
        const float hi_hz = std::exp(log_min + log_step * static_cast<float>(b + 1));
        const int lo_bin = std::max(0, static_cast<int>(std::floor(lo_hz / bin_hz)));
        const int hi_bin = std::min(static_cast<int>(fft_bins.size()) - 1,
                                    static_cast<int>(std::ceil(hi_hz / bin_hz)));

        float peak = 0.0f;
        if (hi_bin >= lo_bin) {
            for (int i = lo_bin; i <= hi_bin; ++i) {
                peak = std::max(peak, fft_bins[static_cast<size_t>(i)]);
            }
        }
        raw[b] = peak;
    }

    // EMA assimétrico: attack rápido (pega transientes), release lento (evita flicker).
    for (size_t b = 0; b < band_count; ++b) {
        if (raw[b] >= m_smoothed[b]) {
            m_smoothed[b] = m_attack * raw[b] + (1.0f - m_attack) * m_smoothed[b];
        } else {
            m_smoothed[b] = m_release * raw[b] + (1.0f - m_release) * m_smoothed[b];
        }
    }
    return m_smoothed;
}

void AudioViewModel::update_data(const domain::models::AudioData& data) {
    QVariantList waveList;
    const int waveStep = std::max(1, static_cast<int>(data.waveform.size()) / 256);
    for (size_t i = 0; i < data.waveform.size(); i += static_cast<size_t>(waveStep)) {
        waveList.append(data.waveform[i]);
    }

    QVariantList specList;
    if (!data.spectrum.empty()) {
        auto bands = transformSpectrum(data.spectrum, kSampleRateHz);
        specList.reserve(static_cast<int>(bands.size()));
        for (const float v : bands) specList.append(v);
    }

    m_waveform = waveList;
    m_spectrum = specList;
    m_rms = data.rms;
    m_peak = data.peak;

    emit audioDataChanged();
}

void AudioViewModel::start() {
    if (!m_provider) return;
    if (m_provider->start_capture()) {
        emit isCapturingChanged();
    }
}

void AudioViewModel::stop() {
    if (!m_provider) return;
    m_provider->stop_capture();
    emit isCapturingChanged();
}

}
