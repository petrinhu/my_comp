#pragma once

#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_classificador_processos.h"

#include <memory>

namespace my_comp::application::vram {

class ServicoClassificarProcesso {
public:
    explicit ServicoClassificarProcesso(std::shared_ptr<domain::vram::IClassificadorProcessos> classificador);

    [[nodiscard]] domain::vram::ResultadoClassificacao
        executar(const domain::vram::Processo& processo) const;

private:
    std::shared_ptr<domain::vram::IClassificadorProcessos> m_classificador;
};

}
