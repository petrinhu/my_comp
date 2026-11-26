#pragma once

#include "domain/vram/interfaces/i_regra_classificacao.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <memory>

namespace my_comp::application::vram {

class RegraFeedback final : public domain::vram::IRegraClassificacao {
public:
    explicit RegraFeedback(std::shared_ptr<domain::vram::IRepositorioFeedback> repo);

    [[nodiscard]] std::optional<domain::vram::ResultadoClassificacao>
        avaliar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::IRepositorioFeedback> m_repo;
};

}
