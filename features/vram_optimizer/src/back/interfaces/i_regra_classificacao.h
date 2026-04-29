#pragma once

#include "back/classificacao/resultado_classificacao.h"
#include "back/entidades/processo.h"

#include <optional>

namespace vramopt::domain {

class IRegraClassificacao {
public:
    IRegraClassificacao() = default;
    IRegraClassificacao(const IRegraClassificacao&) = delete;
    IRegraClassificacao(IRegraClassificacao&&) = delete;
    IRegraClassificacao& operator=(const IRegraClassificacao&) = delete;
    IRegraClassificacao& operator=(IRegraClassificacao&&) = delete;
    virtual ~IRegraClassificacao() = default;

    [[nodiscard]] virtual std::optional<ResultadoClassificacao>
        avaliar(const Processo& processo) const = 0;
};

}
