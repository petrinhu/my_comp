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

using vramopt::application::RegraComportamento;
using vramopt::domain::AmostraJanela;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::LimiaresComportamento;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockAmostradorComportamento;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::deMegabytes(350), 0, {}};
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
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(50),
                                 BytesDeMemoria::deMegabytes(24576),
                                 0.45)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraComportamentoTest, VramAltaMaisAtividadeMinimaClassificaGpu) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(800),
                                 BytesDeMemoria::deMegabytes(8000),
                                 0.10)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
}

TEST(RegraComportamentoTest, VramEComputeBaixosClassificaCpu) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(250),
                                 BytesDeMemoria::deMegabytes(24576),
                                 0.01)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
}

TEST(RegraComportamentoTest, VramAltaMasOciosoClassificaIndeterminado) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(2000),
                                 BytesDeMemoria::deMegabytes(8000),
                                 0.0)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::Indeterminado);
}

TEST(RegraComportamentoTest, VramMediaEComputeMedioClassificaIndeterminado) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(180),
                                 BytesDeMemoria::deMegabytes(24576),
                                 0.12)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::Indeterminado);
}

TEST(RegraComportamentoTest, ClampPisoTrivialAtivoEmGpuPequena) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(50),
                                 BytesDeMemoria::deMegabytes(1024),
                                 0.01)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::CandidatoObvioCpu);
}

TEST(RegraComportamentoTest, JustificativaCitaMetricas) {
    auto amos = std::make_shared<MockAmostradorComportamento>();
    EXPECT_CALL(*amos, amostraJanela(_, _))
        .WillOnce(Return(amostra(BytesDeMemoria::deMegabytes(50),
                                 BytesDeMemoria::deMegabytes(24576),
                                 0.45)));
    RegraComportamento regra{amos, LimiaresComportamento::padrao(), std::chrono::seconds{30}};
    auto r = regra.avaliar(proc());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("comportamento"));
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("VRAM")));
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("compute")));
}
