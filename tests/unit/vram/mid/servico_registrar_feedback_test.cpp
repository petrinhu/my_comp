#include "application/vram/feedback/servico_registrar_feedback.h"
#include "mocks/mock_repositorio_feedback.h"

#include <QDateTime>
#include <QTimeZone>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Field;
using ::testing::Return;
using ::testing::SaveArg;

using vramopt::application::ServicoRegistrarFeedback;
using vramopt::domain::Categoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaFeedback;
using vramopt::tests::MockRepositorioFeedback;

namespace {

const QDateTime k_agora = QDateTime::fromSecsSinceEpoch(1'800'000'000, QTimeZone::utc());

ServicoRegistrarFeedback servico_com(const std::shared_ptr<MockRepositorioFeedback>& repo) {
    return ServicoRegistrarFeedback{repo, [] { return k_agora; }};
}

}

TEST(ServicoRegistrarFeedbackTest, PrimeiroFeedbackCriaEntradaNova) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    const auto chave = ChaveProcesso::deBinario(QStringLiteral("firefox"));
    EXPECT_CALL(*repo, buscar(chave)).WillOnce(Return(std::nullopt));
    EntradaFeedback gravada{.chave = chave};
    EXPECT_CALL(*repo, inserir(_)).WillOnce(SaveArg<0>(&gravada));

    EXPECT_TRUE(servico_com(repo).executar(chave, Categoria::PrecisaGpu, QStringLiteral("WebGL")));
    EXPECT_EQ(gravada.id, 0);  // o repositório atribui o id
    EXPECT_EQ(gravada.chave, chave);
    EXPECT_EQ(gravada.veredito, Categoria::PrecisaGpu);
    EXPECT_EQ(gravada.motivo, QStringLiteral("WebGL"));
    EXPECT_EQ(gravada.criado_em, k_agora);
    EXPECT_EQ(gravada.visto_em, k_agora);
    EXPECT_FALSE(gravada.revisado_em.has_value());
}

TEST(ServicoRegistrarFeedbackTest, RevisarMantemIdECriacaoEMarcaRevisao) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    const auto chave = ChaveProcesso::deBinario(QStringLiteral("firefox"));
    const auto antes = k_agora.addDays(-10);
    EXPECT_CALL(*repo, buscar(chave)).WillOnce(Return(EntradaFeedback{
        .id = 7, .chave = chave, .veredito = Categoria::PrecisaGpu, .motivo = QStringLiteral("a"),
        .criado_em = antes, .visto_em = antes, .revisado_em = std::nullopt}));
    EntradaFeedback gravada{.chave = chave};
    EXPECT_CALL(*repo, inserir(_)).WillOnce(SaveArg<0>(&gravada));

    EXPECT_TRUE(servico_com(repo).executar(chave, Categoria::CandidatoObvioCpu, QStringLiteral("b")));
    EXPECT_EQ(gravada.id, 7);
    EXPECT_EQ(gravada.criado_em, antes);
    EXPECT_EQ(gravada.veredito, Categoria::CandidatoObvioCpu);
    ASSERT_TRUE(gravada.revisado_em.has_value());
    EXPECT_EQ(*gravada.revisado_em, k_agora);
}

TEST(ServicoRegistrarFeedbackTest, IndeterminadoNaoFixaVeredito) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EXPECT_CALL(*repo, inserir(_)).Times(0);
    EXPECT_FALSE(servico_com(repo).executar(ChaveProcesso::deBinario(QStringLiteral("x")),
                                            Categoria::Indeterminado, QString{}));
}

TEST(ServicoRegistrarFeedbackTest, MotivoLongoETruncado) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EXPECT_CALL(*repo, buscar(_)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*repo, inserir(Field(&EntradaFeedback::motivo, ::testing::SizeIs(
        static_cast<int>(ServicoRegistrarFeedback::TAMANHO_MAXIMO_MOTIVO)))));
    EXPECT_TRUE(servico_com(repo).executar(ChaveProcesso::deBinario(QStringLiteral("x")),
                                           Categoria::PrecisaGpu, QString(5000, QLatin1Char('m'))));
}
