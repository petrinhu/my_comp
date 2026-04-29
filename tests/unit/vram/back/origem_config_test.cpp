#include "domain/vram/configuracao/entrada_desktop_exec.h"
#include "domain/vram/configuracao/origem_config.h"

#include <gtest/gtest.h>

using vramopt::domain::CategoriaOrigem;
using vramopt::domain::Confianca;
using vramopt::domain::EntradaDesktopExec;
using vramopt::domain::OrigemConfig;

TEST(OrigemConfigTest, AgregaCampos) {
    OrigemConfig o{
        .categoria = CategoriaOrigem::BrowserChromium,
        .tipo = QStringLiteral("browser-flag"),
        .caminho = QStringLiteral("/home/u/.config/chromium/Local State"),
        .chave = QStringLiteral("hardware_acceleration_mode.enabled"),
        .valor_atual = QStringLiteral("true"),
        .valor_sugerido = QStringLiteral("false"),
        .instrucao_humana = QStringLiteral("desligue aceleração"),
        .confianca = Confianca::Alta,
        .requer_reinicio_app = true,
        .aplicavel_automatico = false,
    };
    EXPECT_EQ(o.categoria, CategoriaOrigem::BrowserChromium);
    EXPECT_EQ(o.tipo, QStringLiteral("browser-flag"));
    EXPECT_EQ(o.confianca, Confianca::Alta);
    EXPECT_TRUE(o.requer_reinicio_app);
    EXPECT_FALSE(o.aplicavel_automatico);
}

TEST(EntradaDesktopExecTest, AgregaCampos) {
    EntradaDesktopExec e{
        .exec = QStringLiteral("prime-run firefox %u"),
        .caminho_arquivo = QStringLiteral("/home/u/.local/share/applications/firefox.desktop"),
    };
    EXPECT_EQ(e.exec, QStringLiteral("prime-run firefox %u"));
    EXPECT_TRUE(e.caminho_arquivo.contains(QStringLiteral("firefox.desktop")));
}
