#include "infrastructure/collectors/vram/composite_gpu_backend.h"

namespace my_comp::infrastructure::vram {

CompositeGpuBackend::CompositeGpuBackend(
    std::vector<std::shared_ptr<domain::vram::IGpuBackend>> backends)
    : m_backends{std::move(backends)} {}

void CompositeGpuBackend::garantir_mapa() const {
    if (m_mapa_pronto) {
        return;
    }
    m_mapa_pronto = true;
}

std::vector<my_comp::domain::vram::GpuInfo>
CompositeGpuBackend::enumerar_gpus() const {
    std::vector<domain::vram::GpuInfo> agregadas;
    m_mapa_ids.clear();

    int proximo_id_global = 0;
    for (std::size_t i = 0; i < m_backends.size(); ++i) {
        const auto gpus = m_backends[i]->enumerar_gpus();
        for (const auto& g : gpus) {
            agregadas.emplace_back(proximo_id_global,
                                   g.fabricante(),
                                   g.modelo(),
                                   g.vram_total());
            m_mapa_ids.push_back(EntradaGpu{.backend_idx = i, .id_interno = g.id()});
            ++proximo_id_global;
        }
    }
    m_mapa_pronto = true;
    return agregadas;
}

std::vector<my_comp::domain::vram::Processo>
CompositeGpuBackend::enumerar_processos_vram(int gpu_id) const {
    if (!m_mapa_pronto) {
        // Recompute mapping from a side-effect-free call to enumerar_gpus
        (void)enumerar_gpus();
    }
    if (gpu_id < 0 || static_cast<std::size_t>(gpu_id) >= m_mapa_ids.size()) {
        return {};
    }
    const auto& entrada = m_mapa_ids[static_cast<std::size_t>(gpu_id)];
    auto processos = m_backends[entrada.backend_idx]->enumerar_processos_vram(entrada.id_interno);

    std::vector<domain::vram::Processo> remapeados;
    remapeados.reserve(processos.size());
    for (const auto& p : processos) {
        remapeados.emplace_back(p.pid(), p.nome(), p.vram_consumida(), gpu_id, p.chaves());
    }
    return remapeados;
}

}
