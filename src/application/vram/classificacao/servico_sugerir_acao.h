#pragma once

#include "application/vram/classificacao/sugestao_acao.h"
#include "domain/vram/classificacao/resultado_classificacao.h"

namespace vramopt::application {

class ServicoSugerirAcao {
public:
    [[nodiscard]] SugestaoAcao
        sugerirPara(const domain::ResultadoClassificacao& resultado) const;
};

}
