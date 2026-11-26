#include "domain/vram/valueobjects/chave_processo.h"

#include <QString>
#include <gtest/gtest.h>

using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::TipoChave;

TEST(ChaveProcessoTest, ConstroiComBinario) {
    auto chave = ChaveProcesso::de_binario(QStringLiteral("firefox"));
    EXPECT_EQ(chave.tipo(), TipoChave::Binario);
    EXPECT_EQ(chave.valor(), QStringLiteral("firefox"));
}

TEST(ChaveProcessoTest, ConstroiComWmClass) {
    auto chave = ChaveProcesso::de_wm_class(QStringLiteral("Firefox"));
    EXPECT_EQ(chave.tipo(), TipoChave::WmClass);
    EXPECT_EQ(chave.valor(), QStringLiteral("Firefox"));
}

TEST(ChaveProcessoTest, ConstroiComDesktopId) {
    auto chave = ChaveProcesso::de_desktop_id(QStringLiteral("firefox.desktop"));
    EXPECT_EQ(chave.tipo(), TipoChave::DesktopId);
    EXPECT_EQ(chave.valor(), QStringLiteral("firefox.desktop"));
}

TEST(ChaveProcessoTest, IgualdadeRequerTipoEValor) {
    EXPECT_EQ(ChaveProcesso::de_binario(QStringLiteral("a")),
              ChaveProcesso::de_binario(QStringLiteral("a")));
    EXPECT_NE(ChaveProcesso::de_binario(QStringLiteral("a")),
              ChaveProcesso::de_wm_class(QStringLiteral("a")));
}

TEST(ChaveProcessoTest, RejeitaValorVazio) {
    EXPECT_THROW((void)ChaveProcesso::de_binario(QString{}), std::invalid_argument);
}
