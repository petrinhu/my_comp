#pragma once
#include <QObject>
#include <QVariantList>
#include "../../domain/interfaces/i_audio_provider.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

class AudioViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList waveform READ waveform NOTIFY audioDataChanged)
    Q_PROPERTY(QVariantList spectrum READ spectrum NOTIFY audioDataChanged)
    Q_PROPERTY(float rms READ rms NOTIFY audioDataChanged)
    Q_PROPERTY(float peak READ peak NOTIFY audioDataChanged)
    Q_PROPERTY(bool isCapturing READ isCapturing NOTIFY isCapturingChanged)
    Q_PROPERTY(float waveformGain READ waveformGain WRITE setWaveformGain NOTIFY gainsChanged)
    Q_PROPERTY(float spectrumGain READ spectrumGain WRITE setSpectrumGain NOTIFY gainsChanged)
    Q_PROPERTY(int spectrumBandCount READ spectrumBandCount WRITE setSpectrumBandCount NOTIFY spectrumConfigChanged)
    Q_PROPERTY(float spectrumMinHz READ spectrumMinHz WRITE setSpectrumMinHz NOTIFY spectrumConfigChanged)
    Q_PROPERTY(float spectrumMaxHz READ spectrumMaxHz WRITE setSpectrumMaxHz NOTIFY spectrumConfigChanged)
    Q_PROPERTY(float spectrumAttack READ spectrumAttack WRITE setSpectrumAttack NOTIFY spectrumConfigChanged)
    Q_PROPERTY(float spectrumRelease READ spectrumRelease WRITE setSpectrumRelease NOTIFY spectrumConfigChanged)

public:
    explicit AudioViewModel(std::shared_ptr<domain::interfaces::IAudioProvider> provider, QObject* parent = nullptr);

    QVariantList waveform() const { return m_waveform; }
    QVariantList spectrum() const { return m_spectrum; }
    float rms() const { return m_rms; }
    float peak() const { return m_peak; }
    bool isCapturing() const { return m_provider ? m_provider->is_capturing() : false; }

    float waveformGain() const { return m_waveform_gain; }
    float spectrumGain() const { return m_spectrum_gain; }
    void setWaveformGain(float g) { if (m_waveform_gain != g) { m_waveform_gain = g; emit gainsChanged(); } }
    void setSpectrumGain(float g) { if (m_spectrum_gain != g) { m_spectrum_gain = g; emit gainsChanged(); } }

    int spectrumBandCount() const { return m_band_count; }
    float spectrumMinHz() const { return m_min_hz; }
    float spectrumMaxHz() const { return m_max_hz; }
    float spectrumAttack() const { return m_attack; }
    float spectrumRelease() const { return m_release; }
    void setSpectrumBandCount(int n);
    void setSpectrumMinHz(float hz);
    void setSpectrumMaxHz(float hz);
    void setSpectrumAttack(float a);
    void setSpectrumRelease(float r);

    // Transforma FFT linear → bandas log-espaçadas com smoothing EMA assimétrico.
    // Exposto para permitir teste determinístico sem depender do pipeline completo.
    std::vector<float> transformSpectrum(const std::vector<float>& fft_bins, float sample_rate_hz);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void audioDataChanged();
    void isCapturingChanged();
    void gainsChanged();
    void spectrumConfigChanged();

private:
    std::shared_ptr<domain::interfaces::IAudioProvider> m_provider;
    QVariantList m_waveform;
    QVariantList m_spectrum;
    float m_rms = 0;
    float m_peak = 0;
    float m_waveform_gain = 2.0f;
    float m_spectrum_gain = 10.0f;

    int m_band_count = 64;
    float m_min_hz = 30.0f;
    float m_max_hz = 20000.0f;
    float m_attack = 0.9f;
    float m_release = 0.15f;
    std::vector<float> m_smoothed;

    void update_data(const domain::models::AudioData& data);
    void reset_smoothing();
};

}
