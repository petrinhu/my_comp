#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <gtest/gtest.h>

using vramopt::domain::BytesDeMemoria;

TEST(BytesDeMemoriaTest, ConstroiComZero) {
    BytesDeMemoria b{0};
    EXPECT_EQ(b.valor(), 0U);
}

TEST(BytesDeMemoriaTest, ConstroiComValorAlto) {
    BytesDeMemoria b{24ULL * 1024 * 1024 * 1024};
    EXPECT_EQ(b.valor(), 24ULL * 1024 * 1024 * 1024);
}

TEST(BytesDeMemoriaTest, FabricaDeMb) {
    auto b = BytesDeMemoria::deMegabytes(512);
    EXPECT_EQ(b.valor(), 512ULL * 1024 * 1024);
}

TEST(BytesDeMemoriaTest, RazaoSobreOutro) {
    BytesDeMemoria total{1024};
    BytesDeMemoria parte{256};
    EXPECT_DOUBLE_EQ(parte.razaoSobre(total), 0.25);
}

TEST(BytesDeMemoriaTest, RazaoSobreZeroDevolveZero) {
    BytesDeMemoria parte{500};
    BytesDeMemoria total{0};
    EXPECT_DOUBLE_EQ(parte.razaoSobre(total), 0.0);
}

TEST(BytesDeMemoriaTest, MaiorOuIgualOrdem) {
    EXPECT_GE(BytesDeMemoria{1000}, BytesDeMemoria{500});
}
