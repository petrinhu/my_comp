#include "application/vram/classificacao/servico_sugerir_acao.h"
#include "domain/vram/classificacao/justificativa.h"
#include "domain/vram/classificacao/resultado_classificacao.h"

#include <gtest/gtest.h>

using my_comp::application::vram::ServicoSugerirAcao;
using my_comp::application::vram::SugestaoAcao;
using my_comp::application::vram::TipoSugestao;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::Justificativa;
using my_comp::domain::vram::ResultadoClassificacao;

TEST(ServicoSugerirAcaoTest, RegraFeedbackDevolveTipoSugestaoNenhuma) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerir_para(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("feedback"), .detalhe = QStringLiteral("id=1")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::Nenhuma);
}

TEST(ServicoSugerirAcaoTest, CatalogoCpuDevolveSugestaoConfigBrowser) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerir_para(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("catalogo"), .detalhe = QStringLiteral("casou binario=firefox")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AjustarConfigDoApp);
    EXPECT_FALSE(sg.descricao.isEmpty());
}

TEST(ServicoSugerirAcaoTest, CategoriaDesktopCpuDevolveSugestaoGenerica) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerir_para(ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{.regra = QStringLiteral("categoria_desktop"), .detalhe = QStringLiteral("WebBrowser")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AjustarConfigDoApp);
}

TEST(ServicoSugerirAcaoTest, ComportamentoIndeterminadoDevolveAvaliacaoManual) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerir_para(ResultadoClassificacao{
        .categoria = Categoria::Indeterminado,
        .justificativa = Justificativa{.regra = QStringLiteral("comportamento"), .detalhe = QStringLiteral("VRAM 5%")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::AvaliacaoManualNecessaria);
}

TEST(ServicoSugerirAcaoTest, PrecisaGpuDevolveNenhuma) {
    ServicoSugerirAcao s;
    SugestaoAcao sg = s.sugerir_para(ResultadoClassificacao{
        .categoria = Categoria::PrecisaGpu,
        .justificativa = Justificativa{.regra = QStringLiteral("biblioteca_compute"), .detalhe = QStringLiteral("libcuda")},
    });
    EXPECT_EQ(sg.tipo, TipoSugestao::Nenhuma);
}
