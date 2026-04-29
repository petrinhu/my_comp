#include "application/vram/configuracao/servico_localizar_config.h"
#include "mocks/mock_config_locator.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::ServicoLocalizarConfig;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::Confianca;
using vramopt::domain::NomeProcesso;
using vramopt::domain::OrigemConfig;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockConfigLocator;

TEST(ServicoLocalizarConfigTest, DelegaAoLocator) {
    auto loc = std::make_shared<MockConfigLocator>();
    OrigemConfig o{
        .categoria = CategoriaOrigem::Firefox,
        .tipo = QStringLiteral("browser-flag"),
        .caminho = QStringLiteral("/x"),
        .chave = QStringLiteral("k"),
        .valor_atual = QStringLiteral("v"),
        .valor_sugerido = QStringLiteral("v2"),
        .instrucao_humana = QStringLiteral("i"),
        .confianca = Confianca::Alta,
        .requer_reinicio_app = true,
        .aplicavel_automatico = false,
    };
    EXPECT_CALL(*loc, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{o}));

    ServicoLocalizarConfig servico{loc};
    Processo p{PID{1}, NomeProcesso{QStringLiteral("x")}, BytesDeMemoria{0}, 0, {}};
    auto r = servico.executar(p);
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r.first().categoria, CategoriaOrigem::Firefox);
}
