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

using my_comp::application::vram::RegraFeedback;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaFeedback;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockRepositorioFeedback;

namespace {

Processo processo_firefox() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(350),
        0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))},
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
    EXPECT_FALSE(regra.avaliar(processo_firefox()).has_value());
}

TEST(RegraFeedbackTest, ClassificaQuandoFeedbackExiste) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EntradaFeedback feedback{
        .id = 42,
        .chave = ChaveProcesso::de_binario(QStringLiteral("firefox")),
        .veredito = Categoria::PrecisaGpu,
        .motivo = QStringLiteral("preciso de aceleração de WebGL"),
        .criado_em = QDateTime::currentDateTimeUtc(),
        .visto_em = QDateTime::currentDateTimeUtc(),
        .revisado_em = std::nullopt,
    };
    EXPECT_CALL(*repo, buscar(_)).WillOnce(Return(feedback));

    RegraFeedback regra{repo};
    auto resultado = regra.avaliar(processo_firefox());
    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(resultado->justificativa.regra, QStringLiteral("feedback"));
}

TEST(RegraFeedbackTest, JustificativaCitaIdEMotivo) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EntradaFeedback feedback{
        .id = 7,
        .chave = ChaveProcesso::de_binario(QStringLiteral("firefox")),
        .veredito = Categoria::CandidatoObvioCpu,
        .motivo = QStringLiteral("desliguei aceleração e não senti diferença"),
        .criado_em = QDateTime::currentDateTimeUtc(),
        .visto_em = QDateTime::currentDateTimeUtc(),
        .revisado_em = std::nullopt,
    };
    EXPECT_CALL(*repo, buscar(_)).WillOnce(Return(feedback));

    RegraFeedback regra{repo};
    auto resultado = regra.avaliar(processo_firefox());
    ASSERT_TRUE(resultado.has_value());
    EXPECT_TRUE(resultado->justificativa.detalhe.contains(QStringLiteral("id=7")));
    EXPECT_TRUE(resultado->justificativa.detalhe.contains(
        QStringLiteral("desliguei aceleração")));
}
