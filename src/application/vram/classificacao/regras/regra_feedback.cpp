#include "application/vram/classificacao/regras/regra_feedback.h"

#include <QString>

namespace my_comp::application::vram {

RegraFeedback::RegraFeedback(std::shared_ptr<domain::vram::IRepositorioFeedback> repo)
    : m_repo{std::move(repo)} {}

std::optional<domain::vram::ResultadoClassificacao>
RegraFeedback::avaliar(const domain::vram::Processo& processo) const {
    for (const auto& chave : processo.chaves()) {
        auto feedback = m_repo->buscar(chave);
        if (feedback.has_value()) {
            return domain::vram::ResultadoClassificacao{
                .categoria = feedback->veredito,
                .justificativa = domain::vram::Justificativa{
                    .regra = QStringLiteral("feedback"),
                    .detalhe = QStringLiteral("id=%1, motivo=%2")
                        .arg(feedback->id)
                        .arg(feedback->motivo),
                },
            };
        }
    }
    return std::nullopt;
}

}
