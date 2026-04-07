#include <gtest/gtest.h>
#include "../../src/domain/interfaces/i_gpu_provider.h"
#include "../../src/infrastructure/collectors/nvidia_gpu_provider.h"
#include <vector>

using namespace my_comp::domain::interfaces;
using namespace my_comp::infrastructure::collectors;

TEST(GpuMonitorAcceptanceTest, DetectsNvidiaGpuAndReturnsStats) {
    auto provider = std::make_unique<NvidiaGpuProvider>();
    auto gpus = provider->get_all_gpus();

    if (!gpus.empty()) {
        for (const auto& gpu : gpus) {
            EXPECT_EQ(gpu.vendor, "NVIDIA");
            EXPECT_FALSE(gpu.model.empty());
            EXPECT_GE(gpu.temp, 0);
            EXPECT_LE(gpu.temp, 110);
            EXPECT_GE(gpu.load, 0);
            EXPECT_LE(gpu.load, 100);
            EXPECT_GT(gpu.vram_total, 0);
        }
    } else {
        GTEST_SKIP() << "Nenhuma GPU NVIDIA detectada ou driver não instalado.";
    }
}
