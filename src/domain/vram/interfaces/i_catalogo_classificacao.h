#pragma once

#include "domain/vram/entidades/entrada_catalogo.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <optional>

namespace my_comp::domain::vram {

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
