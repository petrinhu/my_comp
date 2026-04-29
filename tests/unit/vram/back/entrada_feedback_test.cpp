#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/entidades/entrada_feedback.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <QDateTime>
#include <gtest/gtest.h>

using vramopt::domain::Categoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaFeedback;

TEST(EntradaFeedbackTest, AgregaCampos) {
    auto agora = QDateTime::currentDateTimeUtc();
    EntradaFeedback ef{
        .id          = 42,
        .chave       = ChaveProcesso::deBinario(QStringLiteral("firefox")),
        .veredito    = Categoria::CandidatoObvioCpu,
        .motivo      = QStringLiteral("baixa carga típica"),
        .criado_em   = agora,
        .visto_em    = agora,
        .revisado_em = std::nullopt,
    };
    EXPECT_EQ(ef.id, 42);
    EXPECT_EQ(ef.veredito, Categoria::CandidatoObvioCpu);
    EXPECT_FALSE(ef.revisado_em.has_value());
}
