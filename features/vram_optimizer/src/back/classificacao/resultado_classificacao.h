#pragma once

#include "back/classificacao/categoria.h"
#include "back/classificacao/justificativa.h"

namespace vramopt::domain {

struct ResultadoClassificacao {
    Categoria     categoria{Categoria::Indeterminado};
    Justificativa justificativa{};

    [[nodiscard]] bool operator==(const ResultadoClassificacao&) const noexcept = default;
};

}
