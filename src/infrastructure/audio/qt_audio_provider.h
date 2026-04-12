#pragma once
#include <QObject>
#include <QAudioSource>
#include <QIODevice>
#include <QAudioFormat>
#include "../../domain/interfaces/i_audio_provider.h"
#include <memory>

namespace my_comp::infrastructure::audio {

class QtAudioProvider : public QIODevice, public domain::interfaces::IAudioProvider {
    Q_OBJECT
public:
    explicit QtAudioProvider(QObject* parent = nullptr);
    ~QtAudioProvider() override;

    bool start_capture() override;
    void stop_capture() override;
    bool is_capturing() const override { return m_capturing; }

    // QIODevice
    qint64 readData(char *data, qint64 maxlen) override { Q_UNUSED(data); Q_UNUSED(maxlen); return 0; }
    qint64 writeData(const char *data, qint64 len) override;

private:
    std::unique_ptr<QAudioSource> m_audioSource;
    bool m_capturing = false;
    QAudioFormat m_format;

    void process_samples(const float* samples, int count);
    void stop_capture_impl();
    std::vector<float> calculate_fft(const float* samples, int count);
};

} // namespace my_comp::infrastructure::audio
