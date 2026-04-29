#include "mid/classificacao/regras/mapa_categorias_desktop.h"
#include "mid/classificacao/regras/regra_categoria_desktop.h"
#include "mid/mocks/mock_leitor_desktop_file.h"
#include "back/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::MapaCategoriasDesktop;
using vramopt::application::RegraCategoriaDesktop;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorDesktopFile;

namespace {

Processo processoQualquer() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::deMegabytes(350), 0, {}
    };
}

}

TEST(RegraCategoriaDesktopTest, ClassificaCpuQuandoCategoriaBate) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categoriasDe(_))
        .WillOnce(Return(QStringList{QStringLiteral("WebBrowser")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    auto r = regra.avaliar(processoQualquer());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("categoria_desktop"));
}

TEST(RegraCategoriaDesktopTest, ClassificaGpuQuandoCategoriaBate) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categoriasDe(_))
        .WillOnce(Return(QStringList{QStringLiteral("Game"), QStringLiteral("ActionGame")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    auto r = regra.avaliar(processoQualquer());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraCategoriaDesktopTest, AbstemQuandoCategoriaForaDeAmbosBaldes) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categoriasDe(_))
        .WillOnce(Return(QStringList{QStringLiteral("AudioVideo"), QStringLiteral("Player")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    EXPECT_FALSE(regra.avaliar(processoQualquer()).has_value());
}

TEST(RegraCategoriaDesktopTest, AbstemQuandoLeitorNaoDevolveCategorias) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categoriasDe(_)).WillOnce(Return(QStringList{}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    EXPECT_FALSE(regra.avaliar(processoQualquer()).has_value());
}
