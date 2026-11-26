#pragma once

// Amostrador de comportamento (VRAM-08) que não bloqueia a auditoria: em vez de amostrar
// por conta própria durante a janela, guarda o que as próprias auditorias já coletam.
// Cada enumeração de processos (via GpuBackendComHistorico) registra uma amostra por PID;
// amostra_janela() agrega as amostras dentro da janela pedida. Sem amostra na janela,
// devolve amostras = 0 e a RegraComportamento se abstém.
//
// Limitação: nenhum backend (NVML/i915) expõe hoje utilização de compute por processo,
// então compute_utilizacao_media sai 0.0 e só a VRAM média é real. Como a regra lê
// compute < limiar como ocioso, um processo com VRAM trivial vira CandidatoObvioCpu
// pela VRAM. Fonte futura: nvmlDeviceGetProcessUtilization e os contadores
// drm-engine-* do fdinfo (i915/xe/amdgpu), diferenciados entre duas amostras.
//
// Thread-safe: a auditoria roda em QtConcurrent e pode coincidir com outra leitura.

#include "domain/vram/amostra/amostra_janela.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_amostrador_comportamento.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/pid.h"

#include <QString>

#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <map>
#include <mutex>

namespace my_comp::infrastructure::vram {

class HistoricoAmostrasVram final : public domain::vram::IAmostradorComportamento {
public:
    using Relogio = std::function<std::chrono::steady_clock::time_point()>;

    static constexpr std::size_t          MAXIMO_AMOSTRAS_POR_PID = 120;
    static constexpr std::chrono::seconds RETENCAO{600};

    // Relógio vazio usa std::chrono::steady_clock::now (monotônico: imune a ajuste de hora).
    explicit HistoricoAmostrasVram(Relogio relogio = {});

    void registrar(const domain::vram::Processo& processo, domain::vram::BytesDeMemoria vram_total_gpu);

    [[nodiscard]] domain::vram::AmostraJanela
        amostra_janela(const domain::vram::PID& pid, std::chrono::seconds janela) const override;

    [[nodiscard]] std::size_t pids_rastreados() const;

private:
    struct Amostra {
        std::chrono::steady_clock::time_point quando;
        domain::vram::BytesDeMemoria                vram;
        domain::vram::BytesDeMemoria                vram_total_gpu;
    };
    struct HistoricoDoPid {
        QString             nome;      // PID reaproveitado por outro binário zera o histórico
        std::deque<Amostra> amostras;
    };

    void descartar_antigas(std::chrono::steady_clock::time_point agora);  // exige m_mutex

    Relogio                                              m_relogio;
    mutable std::mutex                                   m_mutex;
    std::map<domain::vram::PID::underlying_t, HistoricoDoPid>  m_por_pid;
};

}
