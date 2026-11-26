#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/entidades/entrada_feedback.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <QDateTime>
#include <gtest/gtest.h>

using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaFeedback;

TEST(EntradaFeedbackTest, AgregaCampos) {
    auto agora = QDateTime::currentDateTimeUtc();
    EntradaFeedback ef{
        .id          = 42,
        .chave       = ChaveProcesso::de_binario(QStringLiteral("firefox")),
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
