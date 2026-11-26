#include "domain/vram/amostra/limiares_comportamento.h"

#include <algorithm>

namespace vramopt::domain {

static constexpr BytesDeMemoria::underlying_t k_bytes_por_mb = 1024ULL * 1024ULL;

LimiaresComportamento LimiaresComportamento::padrao() noexcept {
    return LimiaresComportamento{
        .vram_trivial_percentual = 0.02,
        .vram_trivial_piso       = BytesDeMemoria::de_megabytes(50),
        .vram_alta_percentual    = 0.10,
        .vram_alta_piso          = BytesDeMemoria::de_megabytes(512),
        .compute_idle            = 0.05,
        .compute_sustentado      = 0.30,
    };
}

BytesDeMemoria LimiaresComportamento::vram_trivial_efetivo(BytesDeMemoria vram_total_gpu) const noexcept {
    auto total_mb = vram_total_gpu.valor() / k_bytes_por_mb;
    auto percentual_mb = static_cast<BytesDeMemoria::underlying_t>(
        static_cast<double>(total_mb) * vram_trivial_percentual);
    auto piso_mb = vram_trivial_piso.valor() / k_bytes_por_mb;
    return BytesDeMemoria::de_megabytes(std::max(percentual_mb, piso_mb));
}

BytesDeMemoria LimiaresComportamento::vram_alta_efetivo(BytesDeMemoria vram_total_gpu) const noexcept {
    auto total_mb = vram_total_gpu.valor() / k_bytes_por_mb;
    auto percentual_mb = static_cast<BytesDeMemoria::underlying_t>(
        static_cast<double>(total_mb) * vram_alta_percentual);
    auto piso_mb = vram_alta_piso.valor() / k_bytes_por_mb;
    return BytesDeMemoria::de_megabytes(std::max(percentual_mb, piso_mb));
}

}
