#include "application/vram/configuracao/localizador_composite.h"
#include "mocks/mock_config_locator.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::LocalizadorComposite;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::CategoriaOrigem;
using my_comp::domain::vram::Confianca;
using my_comp::domain::vram::IConfigLocator;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::OrigemConfig;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockConfigLocator;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::de_megabytes(100), 0, {}};
}

OrigemConfig fab(CategoriaOrigem c, const QString& tipo) {
    return OrigemConfig{
        .categoria = c,
        .tipo = tipo,
        .caminho = QStringLiteral("/x"),
        .chave = QStringLiteral("k"),
        .valor_atual = QStringLiteral("v"),
        .valor_sugerido = QStringLiteral("v2"),
        .instrucao_humana = QStringLiteral("i"),
        .confianca = Confianca::Alta,
        .requer_reinicio_app = false,
        .aplicavel_automatico = false,
    };
}

}

TEST(LocalizadorCompositeTest, AgregaResultadosNaOrdem) {
    auto a = std::make_shared<MockConfigLocator>();
    auto b = std::make_shared<MockConfigLocator>();
    EXPECT_CALL(*a, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{
            fab(CategoriaOrigem::BrowserChromium, QStringLiteral("browser-flag"))}));
    EXPECT_CALL(*b, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{
            fab(CategoriaOrigem::PrimeEnv, QStringLiteral("env-var"))}));

    LocalizadorComposite cmp{{a, b}};
    auto r = cmp.localizar(proc());
    ASSERT_EQ(r.size(), 2);
    EXPECT_EQ(r[0].categoria, CategoriaOrigem::BrowserChromium);
    EXPECT_EQ(r[1].categoria, CategoriaOrigem::PrimeEnv);
}

TEST(LocalizadorCompositeTest, IgnoraSublocatorsVazios) {
    auto a = std::make_shared<MockConfigLocator>();
    auto b = std::make_shared<MockConfigLocator>();
    EXPECT_CALL(*a, localizar(_)).WillOnce(Return(QVector<OrigemConfig>{}));
    EXPECT_CALL(*b, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{
            fab(CategoriaOrigem::Firefox, QStringLiteral("browser-flag"))}));

    LocalizadorComposite cmp{{a, b}};
    auto r = cmp.localizar(proc());
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0].categoria, CategoriaOrigem::Firefox);
}

TEST(LocalizadorCompositeTest, VetorVazioDevolveVazio) {
    LocalizadorComposite cmp{{}};
    EXPECT_TRUE(cmp.localizar(proc()).isEmpty());
}

TEST(LocalizadorCompositeTest, ChamaTodosOsSublocatorsMesmoQuePrimeiroEncontrou) {
    auto a = std::make_shared<MockConfigLocator>();
    auto b = std::make_shared<MockConfigLocator>();
    EXPECT_CALL(*a, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{
            fab(CategoriaOrigem::DesktopWrapper, QStringLiteral("desktop-exec"))}));
    EXPECT_CALL(*b, localizar(_)).Times(1);

    LocalizadorComposite cmp{{a, b}};
    static_cast<void>(cmp.localizar(proc()));
}
