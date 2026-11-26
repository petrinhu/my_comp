#pragma once

#include <cstdint>

namespace my_comp::domain::vram {

enum class Categoria : std::uint8_t {
    CandidatoObvioCpu,
    Indeterminado,
    PrecisaGpu,
};

}
