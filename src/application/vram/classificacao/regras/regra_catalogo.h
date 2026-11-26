#pragma once

#include "domain/vram/interfaces/i_catalogo_classificacao.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <memory>

namespace my_comp::application::vram {

class RegraCatalogo final : public domain::vram::IRegraClassificacao {
public:
    explicit RegraCatalogo(std::shared_ptr<domain::vram::ICatalogoClassificacao> cat);

    [[nodiscard]] std::optional<domain::vram::ResultadoClassificacao>
        avaliar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ICatalogoClassificacao> m_catalogo;
};

}
