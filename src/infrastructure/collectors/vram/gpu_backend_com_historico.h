#pragma once

// Decorator de IGpuBackend: repassa o resultado do backend real sem alterá-lo e, de
// carona, registra cada processo enumerado no HistoricoAmostrasVram (VRAM-08). Assim
// cada auditoria alimenta a amostragem sem custo extra nem espera.

#include "domain/vram/interfaces/i_gpu_backend.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "infrastructure/collectors/vram/historico_amostras_vram.h"

#include <map>
#include <memory>
#include <mutex>

namespace my_comp::infrastructure::vram {

class GpuBackendComHistorico final : public domain::vram::IGpuBackend {
public:
    GpuBackendComHistorico(std::shared_ptr<domain::vram::IGpuBackend>  interno,
                           std::shared_ptr<HistoricoAmostrasVram> historico);

    [[nodiscard]] std::vector<domain::vram::GpuInfo> enumerar_gpus() const override;

    [[nodiscard]] std::vector<domain::vram::Processo>
        enumerar_processos_vram(int gpu_id) const override;

private:
    std::shared_ptr<domain::vram::IGpuBackend>          m_interno;
    std::shared_ptr<HistoricoAmostrasVram>        m_historico;
    mutable std::mutex                            m_mutex;
    mutable std::map<int, domain::vram::BytesDeMemoria> m_vram_total_por_gpu;  // da última enumeração
};

}
