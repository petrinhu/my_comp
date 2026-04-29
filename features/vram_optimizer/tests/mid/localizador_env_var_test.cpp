#include "mid/configuracao/sublocators/localizador_env_var.h"
#include "mid/mocks/mock_leitor_environ.h"
#include "back/configuracao/origem_config.h"
#include "back/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::LocalizadorEnvVar;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::Confianca;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorEnviron;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::deMegabytes(100), 0, {}};
}

}

TEST(LocalizadorEnvVarTest, AbstemQuandoEnvironVazio) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    EXPECT_CALL(*leitor, environDe(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    LocalizadorEnvVar loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorEnvVarTest, AbstemQuandoNenhumaEnvRelevante) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    QHash<QString, QString> env{
        {QStringLiteral("HOME"), QStringLiteral("/home/u")},
        {QStringLiteral("PATH"), QStringLiteral("/usr/bin")},
    };
    EXPECT_CALL(*leitor, environDe(_)).WillOnce(Return(env));
    LocalizadorEnvVar loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorEnvVarTest, EmiteOrigemQuandoNvPrimeOffloadAtivo) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    QHash<QString, QString> env{
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("1")},
    };
    EXPECT_CALL(*leitor, environDe(_)).WillOnce(Return(env));
    LocalizadorEnvVar loc{leitor};
    auto origens = loc.localizar(proc());
    ASSERT_EQ(origens.size(), 1);
    const auto& o = origens.first();
    EXPECT_EQ(o.categoria, CategoriaOrigem::PrimeEnv);
    EXPECT_EQ(o.tipo, QStringLiteral("env-var"));
    EXPECT_EQ(o.chave, QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"));
    EXPECT_EQ(o.valor_atual, QStringLiteral("1"));
    EXPECT_EQ(o.confianca, Confianca::Alta);
    EXPECT_TRUE(o.requer_reinicio_app);
}

TEST(LocalizadorEnvVarTest, NaoEmiteQuandoValorEhFalso) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    QHash<QString, QString> env{
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("0")},
        {QStringLiteral("DRI_PRIME"), QStringLiteral("0")},
    };
    EXPECT_CALL(*leitor, environDe(_)).WillOnce(Return(env));
    LocalizadorEnvVar loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorEnvVarTest, EmiteUmaPorEnvVarAtiva) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    QHash<QString, QString> env{
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("1")},
        {QStringLiteral("__VK_LAYER_NV_optimus"), QStringLiteral("NVIDIA_only")},
        {QStringLiteral("DRI_PRIME"), QStringLiteral("1")},
    };
    EXPECT_CALL(*leitor, environDe(_)).WillOnce(Return(env));
    LocalizadorEnvVar loc{leitor};
    auto origens = loc.localizar(proc());
    EXPECT_EQ(origens.size(), 3);
    QStringList chaves;
    for (const auto& o : origens) {
        chaves << o.chave;
    }
    EXPECT_TRUE(chaves.contains(QStringLiteral("__NV_PRIME_RENDER_OFFLOAD")));
    EXPECT_TRUE(chaves.contains(QStringLiteral("__VK_LAYER_NV_optimus")));
    EXPECT_TRUE(chaves.contains(QStringLiteral("DRI_PRIME")));
}
