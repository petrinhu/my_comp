#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"
#include "application/vram/classificacao/regras/regra_categoria_desktop.h"
#include "mocks/mock_leitor_desktop_file.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::MapaCategoriasDesktop;
using my_comp::application::vram::RegraCategoriaDesktop;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockLeitorDesktopFile;

namespace {

Processo processo_qualquer() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(350), 0, {}
    };
}

}

TEST(RegraCategoriaDesktopTest, ClassificaCpuQuandoCategoriaBate) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categorias_de(_))
        .WillOnce(Return(QStringList{QStringLiteral("WebBrowser")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    auto r = regra.avaliar(processo_qualquer());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("categoria_desktop"));
}

TEST(RegraCategoriaDesktopTest, ClassificaGpuQuandoCategoriaBate) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categorias_de(_))
        .WillOnce(Return(QStringList{QStringLiteral("Game"), QStringLiteral("ActionGame")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    auto r = regra.avaliar(processo_qualquer());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraCategoriaDesktopTest, AbstemQuandoCategoriaForaDeAmbosBaldes) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categorias_de(_))
        .WillOnce(Return(QStringList{QStringLiteral("AudioVideo"), QStringLiteral("Player")}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    EXPECT_FALSE(regra.avaliar(processo_qualquer()).has_value());
}

TEST(RegraCategoriaDesktopTest, AbstemQuandoLeitorNaoDevolveCategorias) {
    auto leitor = std::make_shared<MockLeitorDesktopFile>();
    EXPECT_CALL(*leitor, categorias_de(_)).WillOnce(Return(QStringList{}));
    RegraCategoriaDesktop regra{leitor, MapaCategoriasDesktop::padrao()};
    EXPECT_FALSE(regra.avaliar(processo_qualquer()).has_value());
}
