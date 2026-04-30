#pragma once

#include <QString>

#include <cstdint>
#include <optional>
#include <vector>

namespace vramopt::infrastructure {

struct NvmlProcessoInfo {
    std::uint32_t pid;
    std::uint64_t bytes;
};

struct NvmlDeviceInfo {
    QString       nome;
    std::uint64_t vram_total_bytes;
};

class INvmlAdapter {
public:
    INvmlAdapter() = default;
    INvmlAdapter(const INvmlAdapter&) = delete;
    INvmlAdapter(INvmlAdapter&&) = delete;
    INvmlAdapter& operator=(const INvmlAdapter&) = delete;
    INvmlAdapter& operator=(INvmlAdapter&&) = delete;
    virtual ~INvmlAdapter() = default;

    [[nodiscard]] virtual bool isAvailable() const = 0;
    [[nodiscard]] virtual std::uint32_t deviceCount() = 0;
    [[nodiscard]] virtual std::optional<NvmlDeviceInfo> deviceInfo(std::uint32_t idx) = 0;
    [[nodiscard]] virtual std::vector<NvmlProcessoInfo>
        processosCompute(std::uint32_t idx) = 0;
};

}
