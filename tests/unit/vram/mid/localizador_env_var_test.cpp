#include "application/vram/configuracao/sublocators/localizador_env_var.h"
#include "mocks/mock_leitor_environ.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::LocalizadorEnvVar;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::CategoriaOrigem;
using my_comp::domain::vram::Confianca;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockLeitorEnviron;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::de_megabytes(100), 0, {}};
}

}

TEST(LocalizadorEnvVarTest, AbstemQuandoEnvironVazio) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    EXPECT_CALL(*leitor, environ_de(_))
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
    EXPECT_CALL(*leitor, environ_de(_)).WillOnce(Return(env));
    LocalizadorEnvVar loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorEnvVarTest, EmiteOrigemQuandoNvPrimeOffloadAtivo) {
    auto leitor = std::make_shared<MockLeitorEnviron>();
    QHash<QString, QString> env{
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("1")},
    };
    EXPECT_CALL(*leitor, environ_de(_)).WillOnce(Return(env));
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
    EXPECT_CALL(*leitor, environ_de(_)).WillOnce(Return(env));
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
    EXPECT_CALL(*leitor, environ_de(_)).WillOnce(Return(env));
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
