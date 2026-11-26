#include "application/vram/classificacao/regras/regra_comportamento.h"
#include "mocks/mock_amostrador_comportamento.h"
#include "domain/vram/amostra/limiares_comportamento.h"
#include "domain/vram/entidades/processo.h"

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::RegraComportamento;
using my_comp::domain::vram::AmostraJanela;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::LimiaresComportamento;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockAmostradorComportamento;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::de_megabytes(350), 0, {}};
}

AmostraJanela amostra(BytesDeMemoria vram_media,
                      BytesDeMemoria vram_total,
                      double compute) {
    return AmostraJanela{
        .vram_media = vram_media,
        .vram_total_gpu = vram_total,
        .compute_utilizacao_media = compute,
        .amostras = 30,
        .janela = std::chrono::seconds{30},
    };
}

}

TEST(RegraComportamentoTest, ComputeAltoMesmoComVramTrivialClassificaGpu) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(50),
                                 BytesDeMemoria::de_megabytes(24576),
                                 0.45)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraComportamentoTest, VramAltaMaisAtividadeMinimaClassificaGpu) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(800),
                                 BytesDeMemoria::de_megabytes(8000),
                                 0.10)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraComportamentoTest, VramEComputeBaixosClassificaCpu) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(250),
                                 BytesDeMemoria::de_megabytes(24576),
                                 0.01)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
}

TEST(RegraComportamentoTest, VramAltaMasOciosoClassificaIndeterminado) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(2000),
                                 BytesDeMemoria::de_megabytes(8000),
                                 0.0)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::Indeterminado);
}

TEST(RegraComportamentoTest, VramMediaEComputeMedioClassificaIndeterminado) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(180),
                                 BytesDeMemoria::de_megabytes(24576),
                                 0.12)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::Indeterminado);
}

TEST(RegraComportamentoTest, ClampPisoTrivialAtivoEmGpuPequena) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(50),
                                 BytesDeMemoria::de_megabytes(1024),
                                 0.01)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
}

TEST(RegraComportamentoTest, JustificativaCitaMetricas) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostra_janela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::de_megabytes(50),
                                 BytesDeMemoria::de_megabytes(24576),
                                 0.45)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("comportamento"));
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("VRAM")));
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("compute")));
}

// Sem amostras não há base para veredito: com zeros a regra concluiria "VRAM trivial +
// ocioso" e marcaria tudo como CandidatoObvioCpu. Deve se abster e deixar a cascata seguir.
TEST(RegraComportamentoTest, SemAmostrasSeAbstem) {
    auto amostrador = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amostrador, amostra_janela(_, _)).WillOnce(Return(AmostraJanela{}));
    RegraComportamento regra{amostrador, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    EXPECT_FALSE(regra.avaliar(proc()).has_value());
}
