#pragma once

#include "back/classificacao/resultado_classificacao.h"
#include "back/entidades/processo.h"
#include "back/interfaces/i_classificador_processos.h"

#include <memory>

namespace vramopt::application {

class ServicoClassificarProcesso {
public:
    explicit ServicoClassificarProcesso(std::shared_ptr<domain::IClassificadorProcessos> classificador);

    [[nodiscard]] domain::ResultadoClassificacao
        executar(const domain::Processo& processo) const;

private:
    std::shared_ptr<domain::IClassificadorProcessos> m_classificador;
};

}
