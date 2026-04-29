#include "mid/classificacao/regras/regra_catalogo.h"
#include "mid/mocks/mock_catalogo_classificacao.h"
#include "back/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::RegraCatalogo;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaCatalogo;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockCatalogoClassificacao;

namespace {

Processo processoFirefoxComDuasChaves() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::deMegabytes(350),
        0,
        {
            ChaveProcesso::deBinario(QStringLiteral("firefox")),
            ChaveProcesso::deWmClass(QStringLiteral("firefox")),
        }
    };
}

EntradaCatalogo entradaFirefox() {
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
    EXPECT_FALSE(regra.avaliar(processoFirefoxComDuasChaves()).has_value());
}

TEST(RegraCatalogoTest, ClassificaQuandoCatalogoCasa) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    EXPECT_CALL(*cat, buscar(_)).WillOnce(Return(entradaFirefox()));
    RegraCatalogo regra{cat};
    auto r = regra.avaliar(processoFirefoxComDuasChaves());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("catalogo"));
}

TEST(RegraCatalogoTest, JustificativaCitaIdDaEntradaECasamento) {
    auto cat = std::make_shared<MockCatalogoClassificacao>();
    EXPECT_CALL(*cat, buscar(_)).WillOnce(Return(entradaFirefox()));
    RegraCatalogo regra{cat};
    auto r = regra.avaliar(processoFirefoxComDuasChaves());
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("firefox")));
}
