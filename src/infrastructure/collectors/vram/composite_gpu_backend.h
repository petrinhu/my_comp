#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace my_comp::infrastructure::vram {

class CompositeGpuBackend final : public domain::vram::IGpuBackend {
public:
    explicit CompositeGpuBackend(std::vector<std::shared_ptr<domain::vram::IGpuBackend>> backends);

    [[nodiscard]] std::vector<domain::vram::GpuInfo> enumerar_gpus() const override;

    [[nodiscard]] std::vector<domain::vram::Processo>
        enumerar_processos_vram(int gpu_id) const override;

private:
    struct EntradaGpu {
        std::size_t backend_idx;
        int         id_interno;
    };

    std::vector<std::shared_ptr<domain::vram::IGpuBackend>> m_backends;
    mutable std::vector<EntradaGpu> m_mapa_ids;
    mutable bool m_mapa_pronto{false};

    void garantir_mapa() const;
};

}
