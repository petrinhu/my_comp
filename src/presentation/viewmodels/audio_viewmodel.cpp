#include "audio_viewmodel.h"

namespace my_comp::presentation::viewmodels {

AudioViewModel::AudioViewModel(std::shared_ptr<domain::interfaces::IAudioProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    
    m_provider->on_data_ready = [this](const domain::models::AudioData& data) {
        update_data(data);
    };
}

void AudioViewModel::update_data(const domain::models::AudioData& data) {
    // Reduzir resolução para o QML (ex: pegar a cada 10 amostras se for muito denso)
    // Para visualização, 256 ou 512 pontos costumam ser suficientes.
    QVariantList list;
    int step = std::max(1, (int)data.waveform.size() / 256);
    for (size_t i = 0; i < data.waveform.size(); i += step) {
        list.append(data.waveform[i]);
    }

    m_waveform = list;
    m_rms = data.rms;
    m_peak = data.peak;
    
    emit audioDataChanged();
}

void AudioViewModel::start() {
    if (m_provider->start_capture()) {
        emit isCapturingChanged();
    }
}

void AudioViewModel::stop() {
    m_provider->stop_capture();
    emit isCapturingChanged();
}

}
