#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <gtest/gtest.h>

using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;

namespace {

Processo construir_processo_firefox() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(350),
        /*gpu_id=*/0,
        {
            ChaveProcesso::de_binario(QStringLiteral("firefox")),
            ChaveProcesso::de_wm_class(QStringLiteral("firefox")),
        }
    };
}

}

TEST(ProcessoTest, ExpoeAtributosDeConstrucao) {
    auto p = construir_processo_firefox();
    EXPECT_EQ(p.pid().valor(), 1234);
    EXPECT_EQ(p.nome().valor(), QStringLiteral("firefox"));
    EXPECT_EQ(p.vram_consumida(), BytesDeMemoria::de_megabytes(350));
    EXPECT_EQ(p.gpu_id(), 0);
    EXPECT_EQ(p.chaves().size(), 2U);
}

TEST(ProcessoTest, AceitaListaVaziaDeChaves) {
    Processo p{
        PID{42}, NomeProcesso{QStringLiteral("desconhecido")},
        BytesDeMemoria{0}, 0, {}
    };
    EXPECT_TRUE(p.chaves().empty());
}
