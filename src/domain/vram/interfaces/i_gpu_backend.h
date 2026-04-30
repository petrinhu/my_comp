#pragma once

#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"

#include <vector>

namespace vramopt::domain {

class IGpuBackend {
public:
    IGpuBackend() = default;
    IGpuBackend(const IGpuBackend&) = delete;
    IGpuBackend(IGpuBackend&&) = delete;
    IGpuBackend& operator=(const IGpuBackend&) = delete;
    IGpuBackend& operator=(IGpuBackend&&) = delete;
    virtual ~IGpuBackend() = default;

    [[nodiscard]] virtual std::vector<GpuInfo> enumerarGpus() const = 0;

    [[nodiscard]] virtual std::vector<Processo>
        enumerarProcessosVram(int gpu_id) const = 0;
};

}
