#pragma once

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/classificacao/justificativa.h"

namespace vramopt::domain {

struct ResultadoClassificacao {
    Categoria     categoria{Categoria::Indeterminado};
    Justificativa justificativa{};

    [[nodiscard]] bool operator==(const ResultadoClassificacao&) const noexcept = default;
};

}
