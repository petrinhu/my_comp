#include "infrastructure/collectors/vram/gpu_backend_com_historico.h"

#include <utility>

namespace my_comp::infrastructure::vram {

GpuBackendComHistorico::GpuBackendComHistorico(std::shared_ptr<domain::vram::IGpuBackend>  interno,
                                               std::shared_ptr<HistoricoAmostrasVram> historico)
    : m_interno{std::move(interno)}, m_historico{std::move(historico)} {}

std::vector<domain::vram::GpuInfo> GpuBackendComHistorico::enumerar_gpus() const {
    auto gpus = m_interno->enumerar_gpus();
    const std::lock_guard trava{m_mutex};
    m_vram_total_por_gpu.clear();
    for (const auto& gpu : gpus) {
        m_vram_total_por_gpu.insert_or_assign(gpu.id(), gpu.vram_total());
    }
    return gpus;
}

std::vector<domain::vram::Processo> GpuBackendComHistorico::enumerar_processos_vram(int gpu_id) const {
    auto processos = m_interno->enumerar_processos_vram(gpu_id);
    if (!m_historico) {
        return processos;
    }
    domain::vram::BytesDeMemoria total{0};
    {
        const std::lock_guard trava{m_mutex};
        if (const auto it = m_vram_total_por_gpu.find(gpu_id); it != m_vram_total_por_gpu.end()) {
            total = it->second;
        }
    }
    for (const auto& p : processos) {
        m_historico->registrar(p, total);
    }
    return processos;
}

}
