#pragma once

#include "domain/vram/interfaces/i_regra_classificacao.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <memory>

namespace vramopt::application {

class RegraFeedback final : public domain::IRegraClassificacao {
public:
    explicit RegraFeedback(std::shared_ptr<domain::IRepositorioFeedback> repo);

    [[nodiscard]] std::optional<domain::ResultadoClassificacao>
        avaliar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::IRepositorioFeedback> m_repo;
};

}
