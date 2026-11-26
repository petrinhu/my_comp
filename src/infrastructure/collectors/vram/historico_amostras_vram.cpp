#include "infrastructure/collectors/vram/historico_amostras_vram.h"

#include <iterator>
#include <utility>

namespace vramopt::infrastructure {

HistoricoAmostrasVram::HistoricoAmostrasVram(Relogio relogio) : m_relogio{std::move(relogio)} {
    if (!m_relogio) {
        m_relogio = [] { return std::chrono::steady_clock::now(); };
    }
}

void HistoricoAmostrasVram::registrar(const domain::Processo& processo,
                                      domain::BytesDeMemoria vram_total_gpu) {
    const auto agora = m_relogio();
    const std::lock_guard trava{m_mutex};
    descartar_antigas(agora);

    auto& historico = m_por_pid[processo.pid().valor()];
    if (historico.nome != processo.nome().valor()) {
        historico.nome = processo.nome().valor();
        historico.amostras.clear();
    }
    historico.amostras.push_back(Amostra{
        .quando = agora,
        .vram = processo.vramConsumida(),
        .vram_total_gpu = vram_total_gpu,
    });
    while (historico.amostras.size() > MAXIMO_AMOSTRAS_POR_PID) {
        historico.amostras.pop_front();
    }
}

domain::AmostraJanela
HistoricoAmostrasVram::amostraJanela(const domain::PID& pid, std::chrono::seconds janela) const {
    domain::AmostraJanela resultado;
    resultado.janela = janela;

    const auto inicio = m_relogio() - janela;
    const std::lock_guard trava{m_mutex};
    const auto it = m_por_pid.find(pid.valor());
    if (it == m_por_pid.end()) {
        return resultado;
    }

    domain::BytesDeMemoria::underlying_t soma = 0;
    int contagem = 0;
    for (const auto& a : it->second.amostras) {
        if (a.quando < inicio) {
            continue;
        }
        soma += a.vram.valor();
        ++contagem;
        resultado.vram_total_gpu = a.vram_total_gpu;  // amostras em ordem: fica a mais recente
    }
    if (contagem > 0) {
        resultado.vram_media = domain::BytesDeMemoria{
            soma / static_cast<domain::BytesDeMemoria::underlying_t>(contagem)};
        resultado.amostras = contagem;
    }
    // compute_utilizacao_media fica 0.0: sem fonte por processo (ver cabeçalho).
    return resultado;
}

std::size_t HistoricoAmostrasVram::pids_rastreados() const {
    const std::lock_guard trava{m_mutex};
    return m_por_pid.size();
}

void HistoricoAmostrasVram::descartar_antigas(std::chrono::steady_clock::time_point agora) {
    const auto limite = agora - RETENCAO;
    for (auto it = m_por_pid.begin(); it != m_por_pid.end();) {
        auto& amostras = it->second.amostras;
        while (!amostras.empty() && amostras.front().quando < limite) {
            amostras.pop_front();
        }
        it = amostras.empty() ? m_por_pid.erase(it) : std::next(it);
    }
}

}
