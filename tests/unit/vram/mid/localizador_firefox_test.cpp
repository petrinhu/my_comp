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

using vramopt::application::LocalizadorFirefox;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::Confianca;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorArquivoConfig;

namespace {

Processo procFirefox() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(250), 0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))}
    };
}

Processo procBrave() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("brave-browser")},
        BytesDeMemoria::de_megabytes(250), 0, {}
    };
}

QByteArray profilesIniDefault() {
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
    LocalizadorFirefox loc{leitor};
    EXPECT_TRUE(loc.localizar(procBrave()).isEmpty());
}

TEST(LocalizadorFirefoxTest, AbstemQuandoProfilesIniAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(std::nullopt));
    LocalizadorFirefox loc{leitor};
    EXPECT_TRUE(loc.localizar(procFirefox()).isEmpty());
}

TEST(LocalizadorFirefoxTest, AbstemQuandoNenhumProfileDefault) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray sem_default =
        "[Profile0]\nName=p1\nIsRelative=1\nPath=p1.x\n";
    EXPECT_CALL(*leitor, ler(_)).WillOnce(Return(sem_default));
    LocalizadorFirefox loc{leitor};
    EXPECT_TRUE(loc.localizar(procFirefox()).isEmpty());
}

TEST(LocalizadorFirefoxTest, EmiteTresOrigensComPrefsAusente) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profilesIniDefault()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(std::nullopt));

    LocalizadorFirefox loc{leitor};
    auto origens = loc.localizar(procFirefox());
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
        .WillOnce(Return(profilesIniDefault()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(user_js));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(QByteArray{}));

    LocalizadorFirefox loc{leitor};
    auto origens = loc.localizar(procFirefox());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_EQ(origens.first().chave, QStringLiteral("gfx.webrender.all"));
}

TEST(LocalizadorFirefoxTest, EmiteSugestaoComValorAtualLido) {
    auto leitor = std::make_shared<MockLeitorArquivoConfig>();
    QByteArray prefs =
        "user_pref(\"layers.acceleration.disabled\", false);\n";
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("profiles.ini")); })))
        .WillOnce(Return(profilesIniDefault()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(prefs));

    LocalizadorFirefox loc{leitor};
    auto origens = loc.localizar(procFirefox());
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
        .WillOnce(Return(profilesIniDefault()));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("user.js")); })))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*leitor, ler(Truly([](const QString& p){ return p.contains(QStringLiteral("prefs.js")); })))
        .WillOnce(Return(std::nullopt));

    LocalizadorFirefox loc{leitor};
    auto origens = loc.localizar(procFirefox());
    for (const auto& o : origens) {
        EXPECT_TRUE(o.requer_reinicio_app);
        if (o.chave != QStringLiteral("gfx.webrender.all")) {
            EXPECT_EQ(o.valor_sugerido, QStringLiteral("true"));
        }
    }
}
