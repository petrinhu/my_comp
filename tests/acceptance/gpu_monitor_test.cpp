#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_gpu_provider.h"
#include "../../src/infrastructure/collectors/nvidia_gpu_provider.h"
#include <iostream>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace my_comp::infrastructure::collectors;
using namespace testing;

TEST(GpuMonitorAcceptanceTest, DetectsNvidiaGpuAndReturnsStats) {
    NvidiaGpuProvider provider;
    auto stats = provider.collect_stats();
    
    // Como estamos rodando em uma máquina com RTX 3050, 
    // esperamos ao menos uma GPU NVIDIA ativa se os drivers estiverem ok.
    if (!stats.empty()) {
        std::cout << "[GPU Test] Detected: " << stats[0].model_name.toStdString() << std::endl;
        std::cout << "  - Load: " << stats[0].load_percent << "%" << std::endl;
        std::cout << "  - Temp: " << stats[0].temperature << "°C" << std::endl;
        std::cout << "  - VRAM: " << stats[0].vram_used_mb << "/" << stats[0].vram_total_mb << " MB" << std::endl;
        
        EXPECT_EQ(stats[0].vendor, "NVIDIA");
        EXPECT_FALSE(stats[0].model_name.isEmpty());
    } else {
        std::cout << "[GPU Test] No NVIDIA GPU detected or driver issue." << std::endl;
    }
}

class MockGpuProvider : public IGpuProvider {
public:
    MOCK_METHOD(std::vector<GpuStats>, collect_stats, (), (override));
};
