#include "mid/configuracao/sublocators/localizador_browser_chromium.h"
#include "mid/mocks/mock_leitor_arquivo_config.h"
#include "back/configuracao/origem_config.h"
#include "back/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::LocalizadorBrowserChromium;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::Confianca;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorArquivoConfig;

namespace {

Processo procBrave() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("brave-browser")},
        BytesDeMemoria::deMegabytes(350), 0,
        {ChaveProcesso::deBinario(QStringLiteral("brave-browser"))}
    };
}

Processo procFirefox() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::deMegabytes(250), 0,
        {ChaveProcesso::deBinario(QStringLiteral("firefox"))}
    };
}

}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoNomeNaoCasaTabela) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(procFirefox()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoLocalStateAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(std::nullopt));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(procBrave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoJsonInvalido) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(QByteArray{"{not valid json"}));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(procBrave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoAceleracaoJaDesligada) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray json = R"({"hardware_acceleration_mode": {"enabled": false}})";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(json));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(procBrave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, EmiteOrigemQuandoAceleracaoLigada) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray json = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(json));
    LocalizadorBrowserChromium loc{leitor};
    auto origens = loc.localizar(procBrave());
    ASSERT_EQ(origens.size(), 1);
    const auto& o = origens.first();
    EXPECT_EQ(o.categoria, CategoriaOrigem::BrowserChromium);
    EXPECT_EQ(o.tipo, QStringLiteral("browser-flag"));
    EXPECT_EQ(o.chave, QStringLiteral("hardware_acceleration_mode.enabled"));
    EXPECT_EQ(o.valor_atual, QStringLiteral("true"));
    EXPECT_EQ(o.valor_sugerido, QStringLiteral("false"));
    EXPECT_EQ(o.confianca, Confianca::Alta);
    EXPECT_TRUE(o.requer_reinicio_app);
    EXPECT_FALSE(o.aplicavel_automatico);
    EXPECT_TRUE(o.caminho.contains(QStringLiteral("BraveSoftware")));
    EXPECT_TRUE(o.caminho.contains(QStringLiteral("Local State")));
}

TEST(LocalizadorBrowserChromiumTest, EmiteOrigemQuandoChaveAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray json = R"({"other_key": 42})";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(json));
    LocalizadorBrowserChromium loc{leitor};
    auto origens = loc.localizar(procBrave());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_TRUE(origens.first().valor_atual.contains(QStringLiteral("default")));
}
