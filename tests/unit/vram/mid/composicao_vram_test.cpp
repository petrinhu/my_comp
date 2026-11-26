#include "composition/vram_composition.h"

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
