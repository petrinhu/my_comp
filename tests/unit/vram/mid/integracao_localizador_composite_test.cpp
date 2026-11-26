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

using my_comp::application::vram::LocalizadorBrowserChromium;
using my_comp::application::vram::LocalizadorComposite;
using my_comp::application::vram::LocalizadorDesktopWrapper;
using my_comp::application::vram::LocalizadorEnvVar;
using my_comp::application::vram::LocalizadorFirefox;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::CategoriaOrigem;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaDesktopExec;
using my_comp::domain::vram::IConfigLocator;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockLeitorArquivoConfig;
using my_comp::tests::vram::MockLeitorDesktopExec;
using my_comp::tests::vram::MockLeitorEnviron;

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
        std::make_shared<LocalizadorBrowserChromium>(arquivo, QStringLiteral("/home/teste")),
        std::make_shared<LocalizadorFirefox>(arquivo, QStringLiteral("/home/teste")),
        std::make_shared<LocalizadorEnvVar>(env),
        std::make_shared<LocalizadorDesktopWrapper>(desktop),
    };
    return CenarioMontado{arquivo, env, desktop, LocalizadorComposite{std::move(subs)}};
}

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

Processo proc_desconhecido() {
    return Processo{
        PID{99}, NomeProcesso{QStringLiteral("meu-app-foo")},
        BytesDeMemoria::de_megabytes(50), 0, {}
    };
}

auto path_contem(const QString& sub) {
    return Truly([sub](const QString& p) { return p.contains(sub); });
}

}

TEST(IntegracaoLocalizadorCompositeTest, BraveAceleradoEmiteUmaOrigemBrowserChromium) {
    auto c = montar();

    QByteArray local_state = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*c.arquivo, ler(path_contem(QStringLiteral("Local State"))))
        .WillOnce(Return(local_state));
    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(proc_brave());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_EQ(origens.first().categoria, CategoriaOrigem::BrowserChromium);
}

TEST(IntegracaoLocalizadorCompositeTest, FirefoxDefaultEmiteTresOrigens) {
    auto c = montar();

    QByteArray profiles_ini = "[Profile0]\nName=p1\nIsRelative=1\nPath=abc.default-release\nDefault=1\n";
    EXPECT_CALL(*c.arquivo, ler(path_contem(QStringLiteral("profiles.ini"))))
        .WillOnce(Return(profiles_ini));
    EXPECT_CALL(*c.arquivo, ler(path_contem(QStringLiteral("user.js"))))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*c.arquivo, ler(path_contem(QStringLiteral("prefs.js"))))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(proc_firefox());
    ASSERT_EQ(origens.size(), 3);
    for (const auto& o : origens) {
        EXPECT_EQ(o.categoria, CategoriaOrigem::Firefox);
    }
}

TEST(IntegracaoLocalizadorCompositeTest, BraveComPrimeEDesktopWrapperEmiteTresOrigens) {
    auto c = montar();

    QByteArray local_state = R"({"hardware_acceleration_mode": {"enabled": true}})";
    EXPECT_CALL(*c.arquivo, ler(path_contem(QStringLiteral("Local State"))))
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

    auto origens = c.composite.localizar(proc_brave());
    ASSERT_EQ(origens.size(), 3);

    auto tem_categoria = [&](CategoriaOrigem cat) {
        return std::any_of(origens.cbegin(), origens.cend(),
                           [cat](const auto& o) { return o.categoria == cat; });
    };
    EXPECT_TRUE(tem_categoria(CategoriaOrigem::BrowserChromium));
    EXPECT_TRUE(tem_categoria(CategoriaOrigem::PrimeEnv));
    EXPECT_TRUE(tem_categoria(CategoriaOrigem::DesktopWrapper));
}

TEST(IntegracaoLocalizadorCompositeTest, AppDesconhecidoSemConfigsEmiteZero) {
    auto c = montar();

    EXPECT_CALL(*c.env, environ_de(_))
        .WillOnce(Return((QHash<QString, QString>{})));
    EXPECT_CALL(*c.desktop, entrada_de(_))
        .WillOnce(Return(std::nullopt));

    auto origens = c.composite.localizar(proc_desconhecido());
    EXPECT_TRUE(origens.isEmpty());
}
