#pragma once

#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <chrono>

namespace vramopt::domain {

struct AmostraJanela {
    BytesDeMemoria          vram_media{0};
    BytesDeMemoria          vram_total_gpu{0};
    double                  compute_utilizacao_media{0.0};   // 0.0 .. 1.0
    int                     amostras{0};
    std::chrono::seconds    janela{0};

    [[nodiscard]] double razaoVramSobreTotal() const noexcept;
};

}
