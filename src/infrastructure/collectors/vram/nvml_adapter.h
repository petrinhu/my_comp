#pragma once

#include <QString>

#include <cstdint>
#include <optional>
#include <vector>

namespace my_comp::infrastructure::vram {

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

    [[nodiscard]] virtual bool is_available() const = 0;
    [[nodiscard]] virtual std::uint32_t device_count() = 0;
    [[nodiscard]] virtual std::optional<NvmlDeviceInfo> device_info(std::uint32_t idx) = 0;
    [[nodiscard]] virtual std::vector<NvmlProcessoInfo>
        processos_compute(std::uint32_t idx) = 0;
    // Contextos gráficos (browsers, compositores, jogos). Default vazio p/ dublês antigos.
    [[nodiscard]] virtual std::vector<NvmlProcessoInfo>
        processos_graphics(std::uint32_t idx) { (void)idx; return {}; }
};

// NVML_VALUE_NOT_AVAILABLE: usedGpuMemory sem medição (sem permissão, MIG, WDDM).
inline constexpr std::uint64_t NVML_VALOR_NAO_DISPONIVEL = ~0ULL;

}
