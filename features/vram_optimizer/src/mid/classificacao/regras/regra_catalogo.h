#pragma once

#include "back/interfaces/i_catalogo_classificacao.h"
#include "back/interfaces/i_regra_classificacao.h"

#include <memory>

namespace vramopt::application {

class RegraCatalogo final : public domain::IRegraClassificacao {
public:
    explicit RegraCatalogo(std::shared_ptr<domain::ICatalogoClassificacao> cat);

    [[nodiscard]] std::optional<domain::ResultadoClassificacao>
        avaliar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ICatalogoClassificacao> m_catalogo;
};

}
