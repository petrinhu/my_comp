#include "application/vram/classificacao/regras/regra_feedback.h"
#include "mocks/mock_repositorio_feedback.h"
#include "domain/vram/entidades/entrada_feedback.h"
#include "domain/vram/entidades/processo.h"

#include <QDateTime>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::RegraFeedback;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaFeedback;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockRepositorioFeedback;

namespace {

Processo processoFirefox() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::deMegabytes(350),
        0,
        {ChaveProcesso::deBinario(QStringLiteral("firefox"))},
    };
}

}

TEST(RegraFeedbackTest, AbstemSeProcessoNaoTemChaves) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    RegraFeedback regra{repo};

    Processo sem_chaves{
        PID{1}, NomeProcesso{QStringLiteral("x")},
        BytesDeMemoria{0}, 0, {}
    };
    EXPECT_FALSE(regra.avaliar(sem_chaves).has_value());
}

TEST(RegraFeedbackTest, AbstemSeNenhumaChaveCasaNoRepo) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EXPECT_CALL(*repo, buscar(_)).WillRepeatedly(Return(std::nullopt));

    RegraFeedback regra{repo};
    EXPECT_FALSE(regra.avaliar(processoFirefox()).has_value());
}

TEST(RegraFeedbackTest, ClassificaQuandoFeedbackExiste) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EntradaFeedback feedback{
        .id = 42,
        .chave = ChaveProcesso::deBinario(QStringLiteral("firefox")),
        .veredito = Categoria::PrecisaGpu,
        .motivo = QStringLiteral("preciso de aceleração de WebGL"),
        .criado_em = QDateTime::currentDateTimeUtc(),
        .visto_em = QDateTime::currentDateTimeUtc(),
        .revisado_em = std::nullopt,
    };
    EXPECT_CALL(*repo, buscar(_)).WillOnce(Return(feedback));

    RegraFeedback regra{repo};
    auto resultado = regra.avaliar(processoFirefox());
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(resultado->justificativa.regra, QStringLiteral("feedback"));
}

TEST(RegraFeedbackTest, JustificativaCitaIdEMotivo) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EntradaFeedback feedback{
        .id = 7,
        .chave = ChaveProcesso::deBinario(QStringLiteral("firefox")),
        .veredito = Categoria::CandidatoObvioCpu,
        .motivo = QStringLiteral("desliguei aceleração e não senti diferença"),
        .criado_em = QDateTime::currentDateTimeUtc(),
        .visto_em = QDateTime::currentDateTimeUtc(),
        .revisado_em = std::nullopt,
    };
    EXPECT_CALL(*repo, buscar(_)).WillOnce(Return(feedback));

    RegraFeedback regra{repo};
    auto resultado = regra.avaliar(processoFirefox());
    ASSERT_TRUE(resultado.has_value());
    EXPECT_TRUE(resultado->justificativa.detalhe.contains(QStringLiteral("id=7")));
    EXPECT_TRUE(resultado->justificativa.detalhe.contains(
        QStringLiteral("desliguei aceleração")));
}
