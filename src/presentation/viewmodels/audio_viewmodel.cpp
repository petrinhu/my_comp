#include "audio_viewmodel.h"

namespace my_comp::presentation::viewmodels {

AudioViewModel::AudioViewModel(std::shared_ptr<domain::interfaces::IAudioProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    
    m_provider->on_data_ready = [this](const domain::models::AudioData& data) {
        // Garantir que a atualização ocorra na thread da UI (Main Thread)
        QMetaObject::invokeMethod(this, [this, data]() {
            update_data(data);
        }, Qt::QueuedConnection);
    };
}

void AudioViewModel::update_data(const domain::models::AudioData& data) {
    // Reduzir resolução para o QML
    QVariantList waveList;
    int waveStep = std::max(1, (int)data.waveform.size() / 256);
    for (size_t i = 0; i < data.waveform.size(); i += waveStep) {
        waveList.append(data.waveform[i]);
    }

    QVariantList specList;
    if (!data.spectrum.empty()) {
        // Para o espectro, 128 barras costumam ser suficientes
        int specStep = std::max(1, (int)data.spectrum.size() / 128);
        for (size_t i = 0; i < data.spectrum.size(); i += specStep) {
            specList.append(data.spectrum[i]);
        }
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
