#pragma once

#include "infrastructure/collectors/vram/nvml_adapter.h"

#include <memory>

namespace vramopt::infrastructure {

class NvmlAdapterDl final : public INvmlAdapter {
public:
    NvmlAdapterDl();
    ~NvmlAdapterDl() override;

    NvmlAdapterDl(const NvmlAdapterDl&) = delete;
    NvmlAdapterDl(NvmlAdapterDl&&) = delete;
    NvmlAdapterDl& operator=(const NvmlAdapterDl&) = delete;
    NvmlAdapterDl& operator=(NvmlAdapterDl&&) = delete;

    [[nodiscard]] bool isAvailable() const override;
    [[nodiscard]] std::uint32_t deviceCount() override;
    [[nodiscard]] std::optional<NvmlDeviceInfo> deviceInfo(std::uint32_t idx) override;
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processosCompute(std::uint32_t idx) override;
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processosGraphics(std::uint32_t idx) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

[[nodiscard]] std::shared_ptr<INvmlAdapter> criarAdapterNvmlReal();

}
