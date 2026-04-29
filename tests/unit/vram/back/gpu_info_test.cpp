#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"

#include <gtest/gtest.h>

using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Fabricante;
using vramopt::domain::GpuInfo;

TEST(GpuInfoTest, AgregaCampos) {
    GpuInfo gpu{
        /*id=*/0,
        Fabricante::Nvidia,
        QStringLiteral("RTX 4090"),
        BytesDeMemoria::deMegabytes(24576),
    };
    EXPECT_EQ(gpu.id(), 0);
    EXPECT_EQ(gpu.fabricante(), Fabricante::Nvidia);
    EXPECT_EQ(gpu.modelo(), QStringLiteral("RTX 4090"));
    EXPECT_EQ(gpu.vramTotal(), BytesDeMemoria::deMegabytes(24576));
}
