#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

namespace vramopt::infrastructure {

class NullGpuBackend final : public domain::IGpuBackend {
public:
    [[nodiscard]] std::vector<domain::GpuInfo> enumerarGpus() const override {
        return {};
    }

    [[nodiscard]] std::vector<domain::Processo>
        enumerarProcessosVram(int /*gpu_id*/) const override {
        return {};
    }
};

}
