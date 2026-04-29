#include "application/vram/classificacao/servico_classificar_processo.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_classificador_processos.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::ServicoClassificarProcesso;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::IClassificadorProcessos;
using vramopt::domain::Justificativa;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::domain::ResultadoClassificacao;

namespace {

class MockClassificador : public IClassificadorProcessos {
public:
    MOCK_METHOD(ResultadoClassificacao, classificar, (const Processo&), (const, override));
};

}

TEST(ServicoClassificarProcessoTest, DelegaAoClassificadorInterno) {
    auto cls = std::make_shared<MockClassificador>();
    ResultadoClassificacao esperado{
        .categoria = Categoria::PrecisaGpu,
        .justificativa = Justificativa{
            .regra = QStringLiteral("comportamento"),
            .detalhe = QStringLiteral("test"),
        },
    };
    EXPECT_CALL(*cls, classificar(_)).WillOnce(Return(esperado));

    ServicoClassificarProcesso servico{cls};
    auto p = Processo{PID{1}, NomeProcesso{QStringLiteral("x")},
                      BytesDeMemoria{0}, 0, {}};
    auto r = servico.executar(p);
    EXPECT_EQ(r.categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(r.justificativa.regra, QStringLiteral("comportamento"));
}
