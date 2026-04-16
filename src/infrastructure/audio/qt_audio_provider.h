#pragma once
#include <QObject>
#include <QAudioSource>
#include <QIODevice>
#include <QAudioFormat>
#include "../../domain/interfaces/i_audio_provider.h"
#include <memory>
#include <vector>

namespace my_comp::infrastructure::audio {

// Classe auxiliar para receber dados do QAudioSource de forma segura
class AudioSink : public QIODevice {
    Q_OBJECT
public:
    explicit AudioSink(std::function<void(const char*, qint64)> on_data, QObject* parent = nullptr);
    qint64 readData(char* data, qint64 maxlen) override { Q_UNUSED(data); Q_UNUSED(maxlen); return 0; }
    qint64 writeData(const char* data, qint64 len) override;
private:
    std::function<void(const char*, qint64)> m_callback;
};

class QtAudioProvider : public QObject, public domain::interfaces::IAudioProvider {
    Q_OBJECT
public:
    explicit QtAudioProvider(QObject* parent = nullptr);
    ~QtAudioProvider() override;

    bool start_capture() override;
    void stop_capture() override;
    bool is_capturing() const override { return m_capturing; }

    // Exposto para testes (Simula entrada de dados do hardware)
    void writeData(const char* data, qint64 len);

private:
    std::unique_ptr<QAudioSource> m_audioSource;
    std::unique_ptr<AudioSink> m_sink;
    bool m_capturing = false;
    QAudioFormat m_format;

    void process_raw_data(const char* data, qint64 len);
    void process_samples(const float* samples, int count);
    std::vector<float> calculate_fft(const float* samples, int count);
};

} // namespace my_comp::infrastructure::audio
