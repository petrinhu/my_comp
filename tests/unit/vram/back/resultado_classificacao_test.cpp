#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/classificacao/justificativa.h"
#include "domain/vram/classificacao/resultado_classificacao.h"

#include <gtest/gtest.h>

using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::Justificativa;
using my_comp::domain::vram::ResultadoClassificacao;

TEST(ResultadoClassificacaoTest, AgregaCategoriaEJustificativa) {
    ResultadoClassificacao r{
        Categoria::CandidatoObvioCpu,
        Justificativa{QStringLiteral("catalogo"), QStringLiteral("firefox")}
    };
    EXPECT_EQ(r.categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r.justificativa.regra, QStringLiteral("catalogo"));
}
