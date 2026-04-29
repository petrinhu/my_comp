#include "domain/vram/classificacao/justificativa.h"

#include <gtest/gtest.h>

using vramopt::domain::Justificativa;

TEST(JustificativaTest, ConstroiComRegraEDetalhe) {
    Justificativa j{QStringLiteral("catalogo"), QStringLiteral("casou wmClass=firefox")};
    EXPECT_EQ(j.regra, QStringLiteral("catalogo"));
    EXPECT_EQ(j.detalhe, QStringLiteral("casou wmClass=firefox"));
}

TEST(JustificativaTest, IgualdadePorCampos) {
    Justificativa a{QStringLiteral("comportamento"), QStringLiteral("VRAM 1.2%")};
    Justificativa b{QStringLiteral("comportamento"), QStringLiteral("VRAM 1.2%")};
    EXPECT_EQ(a, b);
}
