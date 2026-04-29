#include "domain/vram/valueobjects/nome_processo.h"

#include <QString>
#include <gtest/gtest.h>

#include <stdexcept>

using vramopt::domain::NomeProcesso;

TEST(NomeProcessoTest, ConstroiComStringNaoVazia) {
    NomeProcesso n{QStringLiteral("firefox")};
    EXPECT_EQ(n.valor(), QStringLiteral("firefox"));
}

TEST(NomeProcessoTest, RejeitaStringVazia) {
    EXPECT_THROW(NomeProcesso{QString{}}, std::invalid_argument);
}

TEST(NomeProcessoTest, RejeitaSomenteEspacos) {
    EXPECT_THROW(NomeProcesso{QStringLiteral("   ")}, std::invalid_argument);
}

TEST(NomeProcessoTest, NormalizaTrim) {
    NomeProcesso n{QStringLiteral("  ollama  ")};
    EXPECT_EQ(n.valor(), QStringLiteral("ollama"));
}

TEST(NomeProcessoTest, IgualdadeCaseSensitive) {
    EXPECT_NE(NomeProcesso{QStringLiteral("Firefox")}, NomeProcesso{QStringLiteral("firefox")});
}
