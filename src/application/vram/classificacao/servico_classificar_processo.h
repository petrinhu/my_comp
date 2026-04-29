#pragma once

#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_classificador_processos.h"

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
