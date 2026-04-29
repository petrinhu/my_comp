#pragma once

#include <cstdint>

namespace vramopt::domain {

enum class Categoria : std::uint8_t {
    CandidatoObvioCpu,
    Indeterminado,
    PrecisaGpu,
};

}
