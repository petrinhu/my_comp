#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"
#include "infrastructure/collectors/vram/composite_gpu_backend.h"
#include "mocks/mock_gpu_backend.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

using ::testing::Return;

using vramopt::domain::BytesDeMemoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::Fabricante;
using vramopt::domain::GpuInfo;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::infrastructure::CompositeGpuBackend;
using vramopt::tests::MockGpuBackend;

TEST(CompositeGpuBackendTest, RetornaVazioQuandoSemBackends) {
    CompositeGpuBackend composite{{}};
    EXPECT_TRUE(composite.enumerarGpus().empty());
    EXPECT_TRUE(composite.enumerarProcessosVram(0).empty());
}

TEST(CompositeGpuBackendTest, ConcatenaGpusReatribuindoIdsSequenciais) {
    auto a = std::make_shared<MockGpuBackend>();
    auto b = std::make_shared<MockGpuBackend>();

    GpuInfo g_a0{0, Fabricante::Nvidia, QStringLiteral("RTX A"),
                 BytesDeMemoria::deMegabytes(8192)};
    GpuInfo g_a1{1, Fabricante::Nvidia, QStringLiteral("RTX B"),
                 BytesDeMemoria::deMegabytes(8192)};
    GpuInfo g_b0{0, Fabricante::Intel, QStringLiteral("Iris"),
                 BytesDeMemoria::deMegabytes(2048)};

    EXPECT_CALL(*a, enumerarGpus())
        .WillOnce(Return(std::vector<GpuInfo>{g_a0, g_a1}));
    EXPECT_CALL(*b, enumerarGpus())
        .WillOnce(Return(std::vector<GpuInfo>{g_b0}));

    CompositeGpuBackend composite{{a, b}};
    const auto gpus = composite.enumerarGpus();

    ASSERT_EQ(gpus.size(), 3U);
    EXPECT_EQ(gpus[0].id(), 0);
    EXPECT_EQ(gpus[0].fabricante(), Fabricante::Nvidia);
    EXPECT_EQ(gpus[1].id(), 1);
    EXPECT_EQ(gpus[2].id(), 2);
    EXPECT_EQ(gpus[2].fabricante(), Fabricante::Intel);
}

TEST(CompositeGpuBackendTest, RoteiaProcessosParaBackendCorretoUsandoIdRemapeado) {
    auto a = std::make_shared<MockGpuBackend>();
    auto b = std::make_shared<MockGpuBackend>();

    GpuInfo g_a0{0, Fabricante::Nvidia, QStringLiteral("A"),
                 BytesDeMemoria::deMegabytes(8192)};
    GpuInfo g_b0{0, Fabricante::Intel, QStringLiteral("B"),
                 BytesDeMemoria::deMegabytes(2048)};
    GpuInfo g_b1{1, Fabricante::Intel, QStringLiteral("B2"),
                 BytesDeMemoria::deMegabytes(2048)};

    EXPECT_CALL(*a, enumerarGpus())
        .WillRepeatedly(Return(std::vector<GpuInfo>{g_a0}));
    EXPECT_CALL(*b, enumerarGpus())
        .WillRepeatedly(Return(std::vector<GpuInfo>{g_b0, g_b1}));

    Processo p_a{PID{100}, NomeProcesso{QStringLiteral("a-app")},
                 BytesDeMemoria::deMegabytes(256), 0,
                 {ChaveProcesso::deBinario(QStringLiteral("a-app"))}};
    Processo p_b1{PID{200}, NomeProcesso{QStringLiteral("b-app")},
                  BytesDeMemoria::deMegabytes(128), 1,
                  {ChaveProcesso::deBinario(QStringLiteral("b-app"))}};

    EXPECT_CALL(*a, enumerarProcessosVram(0))
        .WillOnce(Return(std::vector<Processo>{p_a}));
    EXPECT_CALL(*b, enumerarProcessosVram(1))
        .WillOnce(Return(std::vector<Processo>{p_b1}));

    CompositeGpuBackend composite{{a, b}};
    (void)composite.enumerarGpus();

    auto processos_gpu0 = composite.enumerarProcessosVram(0);
    ASSERT_EQ(processos_gpu0.size(), 1U);
    EXPECT_EQ(processos_gpu0[0].pid().valor(), 100);
    EXPECT_EQ(processos_gpu0[0].gpuId(), 0);

    auto processos_gpu2 = composite.enumerarProcessosVram(2);
    ASSERT_EQ(processos_gpu2.size(), 1U);
    EXPECT_EQ(processos_gpu2[0].pid().valor(), 200);
    EXPECT_EQ(processos_gpu2[0].gpuId(), 2);
}

TEST(CompositeGpuBackendTest, GpuIdInvalidoRetornaVazio) {
    auto a = std::make_shared<MockGpuBackend>();
    GpuInfo g_a0{0, Fabricante::Nvidia, QStringLiteral("A"),
                 BytesDeMemoria::deMegabytes(8192)};
    EXPECT_CALL(*a, enumerarGpus())
        .WillRepeatedly(Return(std::vector<GpuInfo>{g_a0}));
    EXPECT_CALL(*a, enumerarProcessosVram(::testing::_)).Times(0);

    CompositeGpuBackend composite{{a}};
    (void)composite.enumerarGpus();

    EXPECT_TRUE(composite.enumerarProcessosVram(99).empty());
    EXPECT_TRUE(composite.enumerarProcessosVram(-1).empty());
}
