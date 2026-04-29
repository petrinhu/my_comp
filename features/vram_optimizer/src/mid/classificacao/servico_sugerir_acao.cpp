#include "mid/classificacao/servico_sugerir_acao.h"

namespace vramopt::application {

SugestaoAcao
ServicoSugerirAcao::sugerirPara(const domain::ResultadoClassificacao& resultado) const {
    if (resultado.categoria == domain::Categoria::PrecisaGpu) {
        return SugestaoAcao{.tipo = TipoSugestao::Nenhuma, .descricao = QString{}};
    }

    if (resultado.justificativa.regra == QStringLiteral("feedback")) {
        return SugestaoAcao{.tipo = TipoSugestao::Nenhuma, .descricao = QString{}};
    }

    if (resultado.categoria == domain::Categoria::CandidatoObvioCpu) {
        if (resultado.justificativa.regra == QStringLiteral("catalogo") ||
            resultado.justificativa.regra == QStringLiteral("categoria_desktop") ||
            resultado.justificativa.regra == QStringLiteral("comportamento")) {
            return SugestaoAcao{
                .tipo = TipoSugestao::AjustarConfigDoApp,
                .descricao = QStringLiteral(
                    "Considere desligar aceleração de hardware no app. "
                    "Detalhes em configuracoesConhecidas do catálogo (quando disponível)."),
            };
        }
    }

    if (resultado.categoria == domain::Categoria::Indeterminado) {
        return SugestaoAcao{
            .tipo = TipoSugestao::AvaliacaoManualNecessaria,
            .descricao = QStringLiteral(
                "Caso ambíguo: revise o uso real do app e marque feedback se quiser fixar veredito."),
        };
    }

    return SugestaoAcao{.tipo = TipoSugestao::Nenhuma, .descricao = QString{}};
}

}
