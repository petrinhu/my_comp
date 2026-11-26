#include "application/vram/classificacao/servico_classificar_processo.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_classificador_processos.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::ServicoClassificarProcesso;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::IClassificadorProcessos;
using my_comp::domain::vram::Justificativa;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::domain::vram::ResultadoClassificacao;

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
