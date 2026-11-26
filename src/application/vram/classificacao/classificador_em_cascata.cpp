#include "application/vram/classificacao/classificador_em_cascata.h"

#include <QString>

namespace my_comp::application::vram {

ClassificadorEmCascata::ClassificadorEmCascata(
    std::vector<std::shared_ptr<domain::vram::IRegraClassificacao>> regras)
    : m_regras{std::move(regras)} {}

domain::vram::ResultadoClassificacao
ClassificadorEmCascata::classificar(const domain::vram::Processo& processo) const {
    for (const auto& regra : m_regras) {
        auto r = regra->avaliar(processo);
        if (r.has_value()) {
            return *r;
        }
    }
    return domain::vram::ResultadoClassificacao{
        .categoria = domain::vram::Categoria::Indeterminado,
        .justificativa = domain::vram::Justificativa{
            .regra = QStringLiteral("nenhuma_regra_decidiu"),
            .detalhe = QStringLiteral("todas as regras da cascata abstiveram-se"),
        },
    };
}

}
