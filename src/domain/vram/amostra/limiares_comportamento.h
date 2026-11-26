#pragma once

#include "domain/vram/valueobjects/bytes_de_memoria.h"

namespace my_comp::domain::vram {

struct LimiaresComportamento {
    double         vram_trivial_percentual{0.0};
    BytesDeMemoria vram_trivial_piso{0};
    double         vram_alta_percentual{0.0};
    BytesDeMemoria vram_alta_piso{0};
    double         compute_idle{0.0};
    double         compute_sustentado{0.0};

    [[nodiscard]] static LimiaresComportamento padrao() noexcept;
    [[nodiscard]] BytesDeMemoria vram_trivial_efetivo(BytesDeMemoria vram_total_gpu) const noexcept;
    [[nodiscard]] BytesDeMemoria vram_alta_efetivo(BytesDeMemoria vram_total_gpu)    const noexcept;
};

}
