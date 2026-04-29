#pragma once

#include "back/interfaces/i_classificador_processos.h"
#include "back/interfaces/i_regra_classificacao.h"

#include <memory>
#include <vector>

namespace vramopt::application {

class ClassificadorEmCascata final : public domain::IClassificadorProcessos {
public:
    explicit ClassificadorEmCascata(std::vector<std::shared_ptr<domain::IRegraClassificacao>> regras);

    [[nodiscard]] domain::ResultadoClassificacao
        classificar(const domain::Processo& processo) const override;

private:
    std::vector<std::shared_ptr<domain::IRegraClassificacao>> m_regras;
};

}
