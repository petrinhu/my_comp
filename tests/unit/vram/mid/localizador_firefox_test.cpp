#include "application/vram/configuracao/sublocators/localizador_firefox.h"
#include "mocks/mock_leitor_arquivo_config.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;
using ::testing::Truly;

using my_comp::application::vram::LocalizadorFirefox;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::CategoriaOrigem;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::Confianca;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockLeitorArquivoConfig;

namespace {

Processo proc_firefox() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(250), 0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))}
    };
}

Processo proc_brave() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("brave-browser")},
        BytesDeMemoria::de_megabytes(250), 0, {}
    };
}

QByteArray profiles_ini_default() {
    return QByteArray{
        "[Profile0]\n"
        "Name=default-release\n"
        "IsRelative=1\n"
        "Path=abc123.default-release\n"
        "Default=1\n"
    };
}

}

TEST(LocalizadorFirefoxTest, AbstemQuandoNomeNaoEhFirefox) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    EXPECT_TRUE(loc.localizar(proc_brave()).isEmpty());
}

TEST(LocalizadorFirefoxTest, AbstemQuandoProfilesIniAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(std::nullopt));
    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    EXPECT_TRUE(loc.localizar(proc_firefox()).isEmpty());
}

TEST(LocalizadorFirefoxTest, AbstemQuandoNenhumProfileDefault) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray sem_default =
        "[Profile0]\nName=p1\nIsRelative=1\nPath=p1.x\n";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(sem_default));
    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    EXPECT_TRUE(loc.localizar(proc_firefox()).isEmpty());
}

TEST(LocalizadorFirefoxTest, EmiteTresOrigensComPrefsAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profiles_ini_default()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(std::nullopt));

    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    auto origens = loc.localizar(proc_firefox());
    ASSERT_EQ(origens.size(), 3);
    QStringList chaves;
    for (const auto& o : origens) {
        chaves << o.chave;
        EXPECT_EQ(o.categoria, CategoriaOrigem::Firefox);
        EXPECT_EQ(o.confianca, Confianca::Alta);
    }
    EXPECT_TRUE(chaves.contains(QStringLiteral("layers.acceleration.disabled")));
    EXPECT_TRUE(chaves.contains(QStringLiteral("webgl.disabled")));
    EXPECT_TRUE(chaves.contains(QStringLiteral("gfx.webrender.all")));
}

TEST(LocalizadorFirefoxTest, NaoEmiteChaveJaCorretamenteSetadaEmUserJs) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray user_js =
        "user_pref(\"layers.acceleration.disabled\", true);\n"
        "user_pref(\"webgl.disabled\", true);\n";
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profiles_ini_default()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(user_js));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(QByteArray{}));

    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    auto origens = loc.localizar(proc_firefox());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_EQ(origens.first().chave, QStringLiteral("gfx.webrender.all"));
}

TEST(LocalizadorFirefoxTest, EmiteSugestaoComValorAtualLido) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray prefs =
        "user_pref(\"layers.acceleration.disabled\", false);\n";
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profiles_ini_default()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(prefs));

    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    auto origens = loc.localizar(proc_firefox());
    bool achou = false;
    for (const auto& o : origens) {
        if (o.chave == QStringLiteral("layers.acceleration.disabled")) {
            EXPECT_EQ(o.valor_atual, QStringLiteral("false"));
            achou = true;
        }
    }
    EXPECT_TRUE(achou);
}

TEST(LocalizadorFirefoxTest, RequerReinicioESugereTrue) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profiles_ini_default()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(std::nullopt));

    LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    auto origens = loc.localizar(proc_firefox());
    for (const auto& o : origens) {
        EXPECT_TRUE(o.requer_reinicio_app);
        if (o.chave != QStringLiteral("gfx.webrender.all")) {
            EXPECT_EQ(o.valor_sugerido, QStringLiteral("true"));
        }
    }
}

// AUDIT-34 (MC-02): o home vem da composição; a camada de aplicação não consulta QDir.
TEST(LocalizadorFirefoxTest, UsaOHomeInjetado) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(QStringLiteral("/home/teste/.mozilla/firefox/profiles.ini")))
        .WillOnce(Return(std::nullopt));
    const LocalizadorFirefox loc{leitor, QStringLiteral("/home/teste")};
    EXPECT_TRUE(loc.localizar(proc_firefox()).isEmpty());
}
