#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

namespace my_comp::infrastructure::vram {

class NullGpuBackend final : public domain::vram::IGpuBackend {
public:
    [[nodiscard]] std::vector<domain::vram::GpuInfo> enumerar_gpus() const override {
        return {};
    }

    [[nodiscard]] std::vector<domain::vram::Processo>
        enumerar_processos_vram(int /*gpu_id*/) const override {
        return {};
    }
};

}
