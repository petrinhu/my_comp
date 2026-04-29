#include "domain/vram/amostra/limiares_comportamento.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <gtest/gtest.h>

using vramopt::domain::BytesDeMemoria;
using vramopt::domain::LimiaresComportamento;

TEST(LimiaresComportamentoTest, FabricaPadraoBateSpec) {
    auto lim = LimiaresComportamento::padrao();
    EXPECT_DOUBLE_EQ(lim.vram_trivial_percentual, 0.02);
    EXPECT_EQ(lim.vram_trivial_piso, BytesDeMemoria::deMegabytes(50));
    EXPECT_DOUBLE_EQ(lim.vram_alta_percentual, 0.10);
    EXPECT_EQ(lim.vram_alta_piso, BytesDeMemoria::deMegabytes(512));
    EXPECT_DOUBLE_EQ(lim.compute_idle, 0.05);
    EXPECT_DOUBLE_EQ(lim.compute_sustentado, 0.30);
}

TEST(LimiaresComportamentoTest, LimiarTrivialEfetivoUsaMaximoEntreClampEPercentual) {
    auto lim = LimiaresComportamento::padrao();
    auto vram_24g = BytesDeMemoria::deMegabytes(24576);
    auto vram_1g  = BytesDeMemoria::deMegabytes(1024);

    EXPECT_EQ(lim.vramTrivialEfetivo(vram_24g), BytesDeMemoria::deMegabytes(491));
    EXPECT_EQ(lim.vramTrivialEfetivo(vram_1g),  BytesDeMemoria::deMegabytes(50));
}

TEST(LimiaresComportamentoTest, LimiarAltoEfetivoUsaMaximoEntreClampEPercentual) {
    auto lim = LimiaresComportamento::padrao();
    auto vram_24g = BytesDeMemoria::deMegabytes(24576);
    auto vram_1g  = BytesDeMemoria::deMegabytes(1024);

    EXPECT_EQ(lim.vramAltaEfetivo(vram_24g), BytesDeMemoria::deMegabytes(2457));
    EXPECT_EQ(lim.vramAltaEfetivo(vram_1g),  BytesDeMemoria::deMegabytes(512));
}
