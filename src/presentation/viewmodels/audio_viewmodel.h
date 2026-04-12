#pragma once
#include <QObject>
#include <QVariantList>
#include "../../domain/interfaces/i_audio_provider.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class AudioViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList waveform READ waveform NOTIFY audioDataChanged)
    Q_PROPERTY(QVariantList spectrum READ spectrum NOTIFY audioDataChanged)
    Q_PROPERTY(float rms READ rms NOTIFY audioDataChanged)
    Q_PROPERTY(float peak READ peak NOTIFY audioDataChanged)
    Q_PROPERTY(bool isCapturing READ isCapturing NOTIFY isCapturingChanged)

public:
    explicit AudioViewModel(std::shared_ptr<domain::interfaces::IAudioProvider> provider, QObject* parent = nullptr);

    QVariantList waveform() const { return m_waveform; }
    QVariantList spectrum() const { return m_spectrum; }
    float rms() const { return m_rms; }
    float peak() const { return m_peak; }
    bool isCapturing() const { return m_provider->is_capturing(); }

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void audioDataChanged();
    void isCapturingChanged();

private:
    std::shared_ptr<domain::interfaces::IAudioProvider> m_provider;
    QVariantList m_waveform;
    QVariantList m_spectrum;
    float m_rms = 0;
    float m_peak = 0;

    void update_data(const domain::models::AudioData& data);
};

}
