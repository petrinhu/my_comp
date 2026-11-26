#include "domain/vram/amostra/amostra_janela.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <chrono>
#include <gtest/gtest.h>

using vramopt::domain::AmostraJanela;
using vramopt::domain::BytesDeMemoria;

TEST(AmostraJanelaTest, AgregaCampos) {
    AmostraJanela am{
        .vram_media               = BytesDeMemoria::de_megabytes(350),
        .vram_total_gpu           = BytesDeMemoria::de_megabytes(24576),
        .compute_utilizacao_media = 0.02,
        .amostras                 = 30,
        .janela                   = std::chrono::seconds{30},
    };
    EXPECT_EQ(am.vram_media, BytesDeMemoria::de_megabytes(350));
    EXPECT_EQ(am.vram_total_gpu, BytesDeMemoria::de_megabytes(24576));
    EXPECT_DOUBLE_EQ(am.compute_utilizacao_media, 0.02);
    EXPECT_EQ(am.amostras, 30);
    EXPECT_EQ(am.janela.count(), 30);
}

TEST(AmostraJanelaTest, RazaoVramSobreTotalUsaBytesDeMemoria) {
    AmostraJanela am{
        .vram_media               = BytesDeMemoria::de_megabytes(512),
        .vram_total_gpu           = BytesDeMemoria::de_megabytes(8192),
        .compute_utilizacao_media = 0.0,
        .amostras                 = 30,
        .janela                   = std::chrono::seconds{30},
    };
    EXPECT_DOUBLE_EQ(am.razao_vram_sobre_total(), 512.0 / 8192.0);
}
