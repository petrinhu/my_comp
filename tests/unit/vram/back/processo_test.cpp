#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <gtest/gtest.h>

using vramopt::domain::BytesDeMemoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;

namespace {

Processo construirProcessoFirefox() {
    return Processo{
        PID{1234},
        NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::deMegabytes(350),
        /*gpu_id=*/0,
        {
            ChaveProcesso::deBinario(QStringLiteral("firefox")),
            ChaveProcesso::deWmClass(QStringLiteral("firefox")),
        }
    };
}

}

TEST(ProcessoTest, ExpoeAtributosDeConstrucao) {
    auto p = construirProcessoFirefox();
    EXPECT_EQ(p.pid().valor(), 1234);
    EXPECT_EQ(p.nome().valor(), QStringLiteral("firefox"));
    EXPECT_EQ(p.vramConsumida(), BytesDeMemoria::deMegabytes(350));
    EXPECT_EQ(p.gpuId(), 0);
    EXPECT_EQ(p.chaves().size(), 2U);
}

TEST(ProcessoTest, AceitaListaVaziaDeChaves) {
    Processo p{
        PID{42}, NomeProcesso{QStringLiteral("desconhecido")},
        BytesDeMemoria{0}, 0, {}
    };
    EXPECT_TRUE(p.chaves().empty());
}
