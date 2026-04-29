#include "mid/classificacao/regras/mapa_categorias_desktop.h"

#include <gtest/gtest.h>

using vramopt::application::MapaCategoriasDesktop;
using vramopt::domain::Categoria;

TEST(MapaCategoriasDesktopTest, PadraoClassificaWebBrowserComoCpu) {
    auto m = MapaCategoriasDesktop::padrao();
    auto v = m.classificarPor({QStringLiteral("WebBrowser")});
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, Categoria::CandidatoObvioCpu);
}

TEST(MapaCategoriasDesktopTest, PadraoClassificaGameComoGpu) {
    auto m = MapaCategoriasDesktop::padrao();
    auto v = m.classificarPor({QStringLiteral("Game"), QStringLiteral("ActionGame")});
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, Categoria::PrecisaGpu);
}

TEST(MapaCategoriasDesktopTest, PadraoAbstemEmPlayer) {
    auto m = MapaCategoriasDesktop::padrao();
    EXPECT_FALSE(m.classificarPor({QStringLiteral("AudioVideo"), QStringLiteral("Player")}).has_value());
}

TEST(MapaCategoriasDesktopTest, ConflitoEntreBaldesAbstem) {
    auto m = MapaCategoriasDesktop::padrao();
    EXPECT_FALSE(m.classificarPor({QStringLiteral("WebBrowser"), QStringLiteral("Game")}).has_value());
}

TEST(MapaCategoriasDesktopTest, ListaVaziaAbstem) {
    auto m = MapaCategoriasDesktop::padrao();
    EXPECT_FALSE(m.classificarPor({}).has_value());
}
