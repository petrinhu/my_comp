#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/entidades/entrada_catalogo.h"

#include <gtest/gtest.h>

using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::EntradaCatalogo;

TEST(EntradaCatalogoTest, AgregaCampos) {
    EntradaCatalogo e{
        .id           = QStringLiteral("firefox"),
        .binarios     = {QStringLiteral("firefox"), QStringLiteral("firefox-esr")},
        .wm_class     = {QStringLiteral("firefox")},
        .desktop_id   = {QStringLiteral("firefox.desktop"), QStringLiteral("org.mozilla.firefox.desktop")},
        .veredito     = Categoria::CandidatoObvioCpu,
        .justificativa = QStringLiteral("Navegador. Aceleração configurável."),
    };
    EXPECT_EQ(e.id, QStringLiteral("firefox"));
    EXPECT_EQ(e.binarios.size(), 2);
    EXPECT_EQ(e.veredito, Categoria::CandidatoObvioCpu);
}
