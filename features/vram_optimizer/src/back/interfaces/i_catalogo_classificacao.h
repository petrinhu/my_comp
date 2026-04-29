#pragma once

#include "back/entidades/entrada_catalogo.h"
#include "back/valueobjects/chave_processo.h"

#include <optional>

namespace vramopt::domain {

class ICatalogoClassificacao {
public:
    ICatalogoClassificacao() = default;
    ICatalogoClassificacao(const ICatalogoClassificacao&) = delete;
    ICatalogoClassificacao(ICatalogoClassificacao&&) = delete;
    ICatalogoClassificacao& operator=(const ICatalogoClassificacao&) = delete;
    ICatalogoClassificacao& operator=(ICatalogoClassificacao&&) = delete;
    virtual ~ICatalogoClassificacao() = default;

    [[nodiscard]] virtual std::optional<EntradaCatalogo>
        buscar(const ChaveProcesso& chave) const = 0;
};

}
