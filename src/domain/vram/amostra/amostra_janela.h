#pragma once

#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <chrono>

namespace my_comp::domain::vram {

struct AmostraJanela {
    BytesDeMemoria          vram_media{0};
    BytesDeMemoria          vram_total_gpu{0};
    double                  compute_utilizacao_media{0.0};   // 0.0 .. 1.0
    // false = a fonte não mede utilização por processo; compute 0.0 então não é "ocioso".
    bool                    compute_medido{false};
    int                     amostras{0};
    std::chrono::seconds    janela{0};

    [[nodiscard]] double razao_vram_sobre_total() const noexcept;
};

}
