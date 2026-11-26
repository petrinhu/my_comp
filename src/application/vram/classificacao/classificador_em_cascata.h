#pragma once

#include "domain/vram/interfaces/i_classificador_processos.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <memory>
#include <vector>

namespace my_comp::application::vram {

class ClassificadorEmCascata final : public domain::vram::IClassificadorProcessos {
public:
    explicit ClassificadorEmCascata(std::vector<std::shared_ptr<domain::vram::IRegraClassificacao>> regras);

    [[nodiscard]] domain::vram::ResultadoClassificacao
        classificar(const domain::vram::Processo& processo) const override;

private:
    std::vector<std::shared_ptr<domain::vram::IRegraClassificacao>> m_regras;
};

}
