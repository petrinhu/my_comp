#pragma once

#include "application/vram/classificacao/sugestao_acao.h"
#include "domain/vram/classificacao/resultado_classificacao.h"

namespace my_comp::application::vram {

class ServicoSugerirAcao {
public:
    [[nodiscard]] SugestaoAcao
        sugerir_para(const domain::vram::ResultadoClassificacao& resultado) const;
};

}
