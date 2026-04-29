#include "mid/classificacao/regras/regra_feedback.h"

#include <QString>

namespace vramopt::application {

RegraFeedback::RegraFeedback(std::shared_ptr<domain::IRepositorioFeedback> repo)
    : m_repo{std::move(repo)} {}

std::optional<domain::ResultadoClassificacao>
RegraFeedback::avaliar(const domain::Processo& processo) const {
    for (const auto& chave : processo.chaves()) {
        auto feedback = m_repo->buscar(chave);
        if (feedback.has_value()) {
            return domain::ResultadoClassificacao{
                .categoria = feedback->veredito,
                .justificativa = domain::Justificativa{
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
