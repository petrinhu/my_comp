#include "composition/vram_composition.h"

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"

#include <gtest/gtest.h>

// A composição única (usada por GUI e CLI) monta a cascata completa; antes só a
// RegraCategoriaDesktop era registrada e as outras 4 regras eram código morto.
TEST(ComposicaoVramTest, CascataRegistraAsCincoRegrasNaOrdemDocumentada) {
    const auto nomes = vramopt::composition::nomes_das_regras_da_cascata();
    const std::vector<QString> esperado = {
        QStringLiteral("feedback"), QStringLiteral("catalogo"), QStringLiteral("categoria_desktop"),
        QStringLiteral("biblioteca_compute"), QStringLiteral("comportamento"),
    };
    EXPECT_EQ(nomes, esperado);
}

TEST(ComposicaoVramTest, ComporServicoDevolveInstanciaUtilizavel) {
    auto servico = vramopt::composition::compor_servico_auditar_vram();
    ASSERT_NE(servico, nullptr);
    (void)servico->executar(); // sem GPU no ambiente de teste: relatório vazio, sem crash
}

// VRAM-06: a RegraCatalogo da composição consulta o catálogo empacotado, não o null object.
TEST(ComposicaoVramTest, CatalogoDaComposicaoEOEmpacotado) {
    const auto catalogo = vramopt::composition::compor_catalogo_classificacao();
    ASSERT_NE(catalogo, nullptr);
    const auto firefox = catalogo->buscar(
        vramopt::domain::ChaveProcesso::deBinario(QStringLiteral("firefox")));
    ASSERT_TRUE(firefox.has_value());
    EXPECT_EQ(firefox->veredito, vramopt::domain::Categoria::CandidatoObvioCpu);
}

// VRAM-07: a RegraFeedback da composição lê o repositório cifrado, não o null object.
TEST(ComposicaoVramTest, RepositorioDeFeedbackDaComposicaoECifrado) {
    const auto repo = vramopt::composition::compor_repositorio_feedback();
    ASSERT_NE(repo, nullptr);
    EXPECT_NE(dynamic_cast<const vramopt::infrastructure::RepositorioFeedbackCifrado*>(repo.get()),
              nullptr);
}

TEST(ComposicaoVramTest, ServicosVramIncluemRegistroDeFeedback) {
    const auto servicos = vramopt::composition::compor_servicos_vram();
    EXPECT_NE(servicos.auditar, nullptr);
    EXPECT_NE(servicos.registrar_feedback, nullptr);
}
