#include "back/valueobjects/chave_processo.h"

#include <QString>
#include <gtest/gtest.h>

using vramopt::domain::ChaveProcesso;
using vramopt::domain::TipoChave;

TEST(ChaveProcessoTest, ConstroiComBinario) {
    auto chave = ChaveProcesso::deBinario(QStringLiteral("firefox"));
    EXPECT_EQ(chave.tipo(), TipoChave::Binario);
    EXPECT_EQ(chave.valor(), QStringLiteral("firefox"));
}

TEST(ChaveProcessoTest, ConstroiComWmClass) {
    auto chave = ChaveProcesso::deWmClass(QStringLiteral("Firefox"));
    EXPECT_EQ(chave.tipo(), TipoChave::WmClass);
    EXPECT_EQ(chave.valor(), QStringLiteral("Firefox"));
}

TEST(ChaveProcessoTest, ConstroiComDesktopId) {
    auto chave = ChaveProcesso::deDesktopId(QStringLiteral("firefox.desktop"));
    EXPECT_EQ(chave.tipo(), TipoChave::DesktopId);
    EXPECT_EQ(chave.valor(), QStringLiteral("firefox.desktop"));
}

TEST(ChaveProcessoTest, IgualdadeRequerTipoEValor) {
    EXPECT_EQ(ChaveProcesso::deBinario(QStringLiteral("a")),
              ChaveProcesso::deBinario(QStringLiteral("a")));
    EXPECT_NE(ChaveProcesso::deBinario(QStringLiteral("a")),
              ChaveProcesso::deWmClass(QStringLiteral("a")));
}

TEST(ChaveProcessoTest, RejeitaValorVazio) {
    EXPECT_THROW((void)ChaveProcesso::deBinario(QString{}), std::invalid_argument);
}
