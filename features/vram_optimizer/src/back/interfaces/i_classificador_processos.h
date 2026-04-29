#pragma once

#include "back/classificacao/resultado_classificacao.h"
#include "back/entidades/processo.h"

namespace vramopt::domain {

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
