#include "application/vram/classificacao/regras/regra_catalogo.h"
#include "mocks/mock_catalogo_classificacao.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::RegraCatalogo;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaCatalogo;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockCatalogoClassificacao;

namespace {

Processo processo_firefox_com_duas_chaves() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(350),
        0,
        {
            ChaveProcesso::de_binario(QStringLiteral("firefox")),
            ChaveProcesso::de_wm_class(QStringLiteral("firefox")),
        }
    };
}

EntradaCatalogo entrada_firefox() {
    return EntradaCatalogo{
        .id = QStringLiteral("firefox"),
        .binarios = {QStringLiteral("firefox")},
        .wm_class = {QStringLiteral("firefox")},
        .desktop_id = {QStringLiteral("firefox.desktop")},
        .veredito = Categoria::CandidatoObvioCpu,
        .justificativa = QStringLiteral("Navegador."),
    };
}

}

TEST(RegraCatalogoTest, AbstemSeSemChaves) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    Processo sem_chaves{PID{1}, NomeProcesso{QStringLiteral("x")},
                        BytesDeMemoria{0}, 0, {}};
    RegraCatalogo regra{cat};
    EXPECT_FALSE(regra.avaliar(sem_chaves).has_value());
}

TEST(RegraCatalogoTest, AbstemSeNenhumaChaveCasa) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    EXPECT_CALL(*cat, buscar(_)).WillRepeatedly(Return(std::nullopt));
    RegraCatalogo regra{cat};
    EXPECT_FALSE(regra.avaliar(processo_firefox_com_duas_chaves()).has_value());
}

TEST(RegraCatalogoTest, ClassificaQuandoCatalogoCasa) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    EXPECT_CALL(*cat, buscar(_)).WillOnce(Return(entrada_firefox()));
    RegraCatalogo regra{cat};
    auto r = regra.avaliar(processo_firefox_com_duas_chaves());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("catalogo"));
}

TEST(RegraCatalogoTest, JustificativaCitaIdDaEntradaECasamento) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    EXPECT_CALL(*cat, buscar(_)).WillOnce(Return(entrada_firefox()));
    RegraCatalogo regra{cat};
    auto r = regra.avaliar(processo_firefox_com_duas_chaves());
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("firefox")));
}
