#pragma once

#include "infrastructure/collectors/vram/nvml_adapter.h"

#include <memory>

namespace my_comp::infrastructure::vram {

class NvmlAdapterDl final : public INvmlAdapter {
public:
    NvmlAdapterDl();
    ~NvmlAdapterDl() override;

    NvmlAdapterDl(const NvmlAdapterDl&) = delete;
    NvmlAdapterDl(NvmlAdapterDl&&) = delete;
    NvmlAdapterDl& operator=(const NvmlAdapterDl&) = delete;
    NvmlAdapterDl& operator=(NvmlAdapterDl&&) = delete;

    [[nodiscard]] bool is_available() const override;
    [[nodiscard]] std::uint32_t device_count() override;
    [[nodiscard]] std::optional<NvmlDeviceInfo> device_info(std::uint32_t idx) override;
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processos_compute(std::uint32_t idx) override;
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processos_graphics(std::uint32_t idx) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

[[nodiscard]] std::shared_ptr<INvmlAdapter> criar_adapter_nvml_real();

}
