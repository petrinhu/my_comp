#pragma once

#include "mid/classificacao/sugestao_acao.h"
#include "back/classificacao/resultado_classificacao.h"

namespace vramopt::application {

class ServicoSugerirAcao {
public:
    [[nodiscard]] SugestaoAcao
        sugerirPara(const domain::ResultadoClassificacao& resultado) const;
};

}
