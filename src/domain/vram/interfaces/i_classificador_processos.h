#pragma once

#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/entidades/processo.h"

namespace my_comp::domain::vram {

class IClassificadorProcessos {
public:
    IClassificadorProcessos() = default;
    IClassificadorProcessos(const IClassificadorProcessos&) = delete;
    IClassificadorProcessos(IClassificadorProcessos&&) = delete;
    IClassificadorProcessos& operator=(const IClassificadorProcessos&) = delete;
    IClassificadorProcessos& operator=(IClassificadorProcessos&&) = delete;
    virtual ~IClassificadorProcessos() = default;

    [[nodiscard]] virtual ResultadoClassificacao
        classificar(const Processo& processo) const = 0;
};

}
