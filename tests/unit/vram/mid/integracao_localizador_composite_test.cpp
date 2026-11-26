#include "application/vram/configuracao/localizador_composite.h"
#include "application/vram/configuracao/sublocators/localizador_browser_chromium.h"
#include "application/vram/configuracao/sublocators/localizador_desktop_wrapper.h"
#include "application/vram/configuracao/sublocators/localizador_env_var.h"
#include "application/vram/configuracao/sublocators/localizador_firefox.h"
#include "mocks/mock_leitor_arquivo_config.h"
#include "mocks/mock_leitor_desktop_exec.h"
#include "mocks/mock_leitor_environ.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

using ::testing::_;
using ::testing::Return;
using ::testing::Truly;

using vramopt::application::LocalizadorBrowserChromium;
using vramopt::application::LocalizadorComposite;
using vramopt::application::LocalizadorDesktopWrapper;
using vramopt::application::LocalizadorEnvVar;
using vramopt::application::LocalizadorFirefox;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaDesktopExec;
using vramopt::domain::IConfigLocator;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorArquivoConfig;
using vramopt::tests::MockLeitorDesktopExec;
using vramopt::tests::MockLeitorEnviron;

namespace {

struct CenarioMontado {
    std::shared_ptr<MockLeitorArquivoConfig> arquivo;
    std::shared_ptr<MockLeitorEnviron>       env;
    std::shared_ptr<MockLeitorDesktopExec>   desktop;
    LocalizadorComposite                     composite;
};

CenarioMontado montar() {
    auto arquivo = std::make_shared<MockLeitorArquivoConfig>();
    auto env     = std::make_shared<MockLeitorEnviron>();
    auto desktop = std::make_shared<MockLeitorDesktopExec>();

    std::vector<std::shared_ptr<IConfigLocator>> subs = {
        std::make_shared<LocalizadorBrowserChromium>(arquivo),
        std::make_shared<LocalizadorFirefox>(arquivo),
        std::make_shared<LocalizadorEnvVar>(env),
        std::make_shared<LocalizadorDesktopWrapper>(desktop),
    };
    return CenarioMontado{arquivo, env, desktop, LocalizadorComposite{std::move(subs)}};
}

Processo procBrave() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("brave-browser")},
        BytesDeMemoria::de_megabytes(350), 0,
        {ChaveProcesso::de_binario(QStringLiteral("brave-browser"))}
    };
}

Processo procFirefox() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(250), 0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))}
    };
}

Processo procDesconhecido() {
    return Processo{
        PID{99}, NomeProcesso{QStringLiteral("meu-app-foo")},
        BytesDeMemoria::de_megabytes(50), 0, {}
    };
}

auto pathContem(const QString& sub) {
    return Truly([sub](const QString& p) { return p.contains(sub); });
}

}

TEST(IntegracaoLocalizadorCompositeTest, BraveAceleradoEmiteUmaOrigemBrowserChromium) {
    auto c = montar();

    QByteArray local_state = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*c.arquivo, ler(pathContem(QStringLiteral("Local State"))))
        .WillOnce(Return(local_state));
    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(procBrave());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_EQ(origens.first().categoria, CategoriaOrigem::BrowserChromium);
}

TEST(IntegracaoLocalizadorCompositeTest, FirefoxDefaultEmiteTresOrigens) {
    auto c = montar();

    QByteArray profiles_ini = "[Profile0]\nName=p1\nIsRelative=1\nPath=abc.default-release\nDefault=1\n";
    EXPECT_CALL(*c.arquivo, ler(pathContem(QStringLiteral("profiles.ini"))))
        .WillOnce(Return(profiles_ini));
    EXPECT_CALL(*c.arquivo, ler(pathContem(QStringLiteral("user.js"))))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*c.arquivo, ler(pathContem(QStringLiteral("prefs.js"))))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(procFirefox());
    ASSERT_EQ(origens.size(), 3);
    for (const auto& o : origens) {
        EXPECT_EQ(o.categoria, CategoriaOrigem::Firefox);
    }
}

TEST(IntegracaoLocalizadorCompositeTest, BraveComPrimeEDesktopWrapperEmiteTresOrigens) {
    auto c = montar();

    QByteArray local_state = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*c.arquivo, ler(pathContem(QStringLiteral("Local State"))))
        .WillOnce(Return(local_state));
    QHash<QString, QString> env{
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("1")},
    };
    EXPECT_CALL(*c.env, environ_de(_)).WillOnce(Return(env));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(EntradaDesktopExec{
            .exec = QStringLiteral("prime-run brave-browser %u"),
            .caminho_arquivo = QStringLiteral("/home/u/.local/share/applications/brave-browser.desktop"),
        }));

    auto origens = c.composite.localizar(procBrave());
    ASSERT_EQ(origens.size(), 3);

    auto temCategoria = [&](CategoriaOrigem cat) {
        return std::any_of(origens.cbegin(), origens.cend(),
                           [cat](const auto& o) { return o.categoria == cat; });
    };
    EXPECT_TRUE(temCategoria(CategoriaOrigem::BrowserChromium));
    EXPECT_TRUE(temCategoria(CategoriaOrigem::PrimeEnv));
    EXPECT_TRUE(temCategoria(CategoriaOrigem::DesktopWrapper));
}

TEST(IntegracaoLocalizadorCompositeTest, AppDesconhecidoSemConfigsEmiteZero) {
    auto c = montar();

    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(procDesconhecido());
    EXPECT_TRUE(origens.isEmpty());
}
