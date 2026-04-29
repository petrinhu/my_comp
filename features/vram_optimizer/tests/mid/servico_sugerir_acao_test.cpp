#include "mid/classificacao/servico_sugerir_acao.h"
#include "back/classificacao/justificativa.h"
#include "back/classificacao/resultado_classificacao.h"

#include <gtest/gtest.h>

using vramopt::application::ServicoSugerirAcao;
using vramopt::application::SugestaoAcao;
using vramopt::application::TipoSugestao;
using vramopt::domain::Categoria;
using vramopt::domain::Justificativa;
using vramopt::domain::ResultadoClassificacao;

TEST(ServicoSugerirAcaoTest, RegraFeedbackDevolveTipoSugestaoNenhuma) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerirPara(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("feedback"), .detalhe = QStringLiteral("id=1")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::Nenhuma);
}

TEST(ServicoSugerirAcaoTest, CatalogoCpuDevolveSugestaoConfigBrowser) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerirPara(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("catalogo"), .detalhe = QStringLiteral("casou binario=firefox")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AjustarConfigDoApp);
    EXPECT_FALSE(sg.descricao.isEmpty());
}

TEST(ServicoSugerirAcaoTest, CategoriaDesktopCpuDevolveSugestaoGenerica) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerirPara(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("categoria_desktop"), .detalhe = QStringLiteral("WebBrowser")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AjustarConfigDoApp);
}

TEST(ServicoSugerirAcaoTest, ComportamentoIndeterminadoDevolveAvaliacaoManual) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerirPara(ResultadoClassificacao{
        .categoria = Categoria::Indeterminado,
        .justificativa = Justificativa{.regra = QStringLiteral("comportamento"), .detalhe = QStringLiteral("VRAM 5%")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AvaliacaoManualNecessaria);
}

TEST(ServicoSugerirAcaoTest, PrecisaGpuDevolveNenhuma) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerirPara(ResultadoClassificacao{
        .categoria = Categoria::PrecisaGpu,
        .justificativa = Justificativa{.regra = QStringLiteral("biblioteca_compute"), .detalhe = QStringLiteral("libcuda")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::Nenhuma);
}
