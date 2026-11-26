#include "application/vram/configuracao/sublocators/localizador_desktop_wrapper.h"
#include "mocks/mock_leitor_desktop_exec.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::LocalizadorDesktopWrapper;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::Confianca;
using vramopt::domain::EntradaDesktopExec;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorDesktopExec;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("firefox")},
                    BytesDeMemoria::de_megabytes(100), 0, {}};
}

EntradaDesktopExec entrada(QString exec, QString caminho =
        QStringLiteral("/home/u/.local/share/applications/firefox.desktop")) {
    return EntradaDesktopExec{.exec = std::move(exec), .caminho_arquivo = std::move(caminho)};
}

}

TEST(LocalizadorDesktopWrapperTest, AbstemQuandoLeitorDevolveNullopt) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_)).WillOnce(Return(std::nullopt));
    LocalizadorDesktopWrapper loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorDesktopWrapperTest, AbstemQuandoExecSemWrapper) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_))
        .WillOnce(Return(entrada(QStringLiteral("firefox %u"))));
    LocalizadorDesktopWrapper loc{leitor};
    EXPECT_TRUE(loc.localizar(proc()).isEmpty());
}

TEST(LocalizadorDesktopWrapperTest, EmiteOrigemQuandoExecComecaComPrimeRun) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_))
        .WillOnce(Return(entrada(QStringLiteral("prime-run firefox %u"))));
    LocalizadorDesktopWrapper loc{leitor};
    auto origens = loc.localizar(proc());
    ASSERT_EQ(origens.size(), 1);
    const auto& o = origens.first();
    EXPECT_EQ(o.categoria, CategoriaOrigem::DesktopWrapper);
    EXPECT_EQ(o.tipo, QStringLiteral("desktop-exec"));
    EXPECT_EQ(o.chave, QStringLiteral("Exec"));
    EXPECT_EQ(o.valor_atual, QStringLiteral("prime-run firefox %u"));
    EXPECT_EQ(o.valor_sugerido, QStringLiteral("firefox %u"));
    EXPECT_EQ(o.confianca, Confianca::Alta);
    EXPECT_TRUE(o.requer_reinicio_app);
    EXPECT_TRUE(o.caminho.contains(QStringLiteral(".desktop")));
}

TEST(LocalizadorDesktopWrapperTest, EmiteConfiancaMediaParaGamemoderun) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_))
        .WillOnce(Return(entrada(QStringLiteral("gamemoderun firefox %u"))));
    LocalizadorDesktopWrapper loc{leitor};
    auto origens = loc.localizar(proc());
    ASSERT_EQ(origens.size(), 1);
    EXPECT_EQ(origens.first().confianca, Confianca::Media);
}

TEST(LocalizadorDesktopWrapperTest, ReconheceOptirun) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_))
        .WillOnce(Return(entrada(QStringLiteral("optirun firefox"))));
    LocalizadorDesktopWrapper loc{leitor};
    EXPECT_EQ(loc.localizar(proc()).size(), 1);
}

TEST(LocalizadorDesktopWrapperTest, IgnoraEspacosIniciaisDoExec) {
    auto leitor = std::make_shared<MockLeitorDesktopExec>();
    EXPECT_CALL(*leitor, entrada_de(_))
        .WillOnce(Return(entrada(QStringLiteral("   prime-run firefox  "))));
    LocalizadorDesktopWrapper loc{leitor};
    EXPECT_EQ(loc.localizar(proc()).size(), 1);
}
