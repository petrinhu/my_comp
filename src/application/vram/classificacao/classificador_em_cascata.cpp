#include "application/vram/classificacao/classificador_em_cascata.h"

#include <QString>

namespace vramopt::application {

ClassificadorEmCascata::ClassificadorEmCascata(
    std::vector<std::shared_ptr<domain::IRegraClassificacao>> regras)
    : m_regras{std::move(regras)} {}

domain::ResultadoClassificacao
ClassificadorEmCascata::classificar(const domain::Processo& processo) const {
    for (const auto& regra : m_regras) {
        auto r = regra->avaliar(processo);
        if (r.has_value()) {
            return *r;
        }
    }
    return domain::ResultadoClassificacao{
        .categoria = domain::Categoria::Indeterminado,
        .justificativa = domain::Justificativa{
            .regra = QStringLiteral("nenhuma_regra_decidiu"),
            .detalhe = QStringLiteral("todas as regras da cascata abstiveram-se"),
        },
    };
}

}
