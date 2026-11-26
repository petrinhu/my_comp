#include "application/vram/configuracao/sublocators/localizador_browser_chromium.h"
#include "mocks/mock_leitor_arquivo_config.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::LocalizadorBrowserChromium;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::CategoriaOrigem;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::Confianca;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockLeitorArquivoConfig;

namespace {

Processo proc_brave() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("brave-browser")},
        BytesDeMemoria::de_megabytes(350), 0,
        {ChaveProcesso::de_binario(QStringLiteral("brave-browser"))}
    };
}

Processo proc_firefox() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(250), 0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))}
    };
}

}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoNomeNaoCasaTabela) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(proc_firefox()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoLocalStateAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(std::nullopt));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(proc_brave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoJsonInvalido) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(QByteArray{"{not valid json"}));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(proc_brave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, AbstemQuandoAceleracaoJaDesligada) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray json = R"({"hardware_acceleration_mode": {"enabled": false}})";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(json));
    LocalizadorBrowserChromium loc{leitor};
    EXPECT_TRUE(loc.localizar(proc_brave()).isEmpty());
}

TEST(LocalizadorBrowserChromiumTest, EmiteOrigemQuandoAceleracaoLigada) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray json = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(json));
    LocalizadorBrowserChromium loc{leitor};
    auto origens = loc.localizar(proc_brave());
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
    auto origens = loc.localizar(proc_brave());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_TRUE(origens.first().valor_atual.contains(QStringLiteral("default")));
}
