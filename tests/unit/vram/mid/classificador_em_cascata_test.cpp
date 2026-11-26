#include "application/vram/classificacao/classificador_em_cascata.h"
#include "mocks/mock_regra_classificacao.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

using ::testing::_;
using ::testing::Return;

using vramopt::application::ClassificadorEmCascata;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::Justificativa;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::domain::ResultadoClassificacao;
using vramopt::tests::MockRegraClassificacao;

namespace {

Processo proc() {
    return Processo{PID{1234}, NomeProcesso{QStringLiteral("x")},
                    BytesDeMemoria::de_megabytes(100), 0, {}};
}

ResultadoClassificacao result(Categoria c, const QString& regra) {
    return ResultadoClassificacao{
        .categoria = c,
        .justificativa = Justificativa{.regra = regra, .detalhe = QStringLiteral("d")},
    };
}

}

TEST(ClassificadorEmCascataTest, RegraQueDecideInterrompeCascata) {
    auto r1 = std::make_shared<MockRegraClassificacao>();
    auto r2 = std::make_shared<MockRegraClassificacao>();
    auto r3 = std::make_shared<MockRegraClassificacao>();

    EXPECT_CALL(*r1, avaliar(_)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*r2, avaliar(_))
        .WillOnce(Return(result(Categoria::PrecisaGpu, QStringLiteral("regra2"))));
    EXPECT_CALL(*r3, avaliar(_)).Times(0);

    ClassificadorEmCascata cls{{r1, r2, r3}};
    auto v = cls.classificar(proc());
    EXPECT_EQ(v.categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("regra2"));
}

TEST(ClassificadorEmCascataTest, OrdemDoVetorImporta) {
    auto a = std::make_shared<MockRegraClassificacao>();
    auto b = std::make_shared<MockRegraClassificacao>();
    EXPECT_CALL(*a, avaliar(_))
        .WillOnce(Return(result(Categoria::CandidatoObvioCpu, QStringLiteral("a"))));
    EXPECT_CALL(*b, avaliar(_)).Times(0);

    ClassificadorEmCascata cls{{a, b}};
    EXPECT_EQ(cls.classificar(proc()).justificativa.regra, QStringLiteral("a"));
}

TEST(ClassificadorEmCascataTest, TodasAbstemCaiEmIndeterminado) {
    auto r1 = std::make_shared<MockRegraClassificacao>();
    auto r2 = std::make_shared<MockRegraClassificacao>();
    EXPECT_CALL(*r1, avaliar(_)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*r2, avaliar(_)).WillOnce(Return(std::nullopt));

    ClassificadorEmCascata cls{{r1, r2}};
    auto v = cls.classificar(proc());
    EXPECT_EQ(v.categoria, Categoria::Indeterminado);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("nenhuma_regra_decidiu"));
}

TEST(ClassificadorEmCascataTest, VetorVazioCaiEmIndeterminado) {
    ClassificadorEmCascata cls{{}};
    auto v = cls.classificar(proc());
    EXPECT_EQ(v.categoria, Categoria::Indeterminado);
}
