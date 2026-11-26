#pragma once

#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/entidades/processo.h"

#include <optional>

namespace my_comp::domain::vram {

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
