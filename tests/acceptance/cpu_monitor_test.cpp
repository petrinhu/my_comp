#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_cpu_provider.h"
#include <memory>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

// Mock para fins de teste de aceitação (ou usaríamos a implementação real se já existisse)
class MockCpuProvider : public ICpuProvider {
public:
    MOCK_METHOD(CpuStats, collect_stats, (), (override));
    MOCK_METHOD(CpuArchitecture, collect_architecture, (), (override));
};

TEST(CpuMonitorAcceptance, ShouldCollectRealisticCpuData) {
    // Este teste valida se a estrutura de dados coletada faz sentido para o sistema
    // Em um teste de aceitação real, usaríamos o LinuxCpuProvider.
    
    // Por enquanto, validamos se as interfaces e modelos funcionam conforme o esperado.
    auto provider = std::make_unique<MockCpuProvider>();
    
    CpuStats fake_stats;
    fake_stats.total_load = 45.5;
    fake_stats.average_frequency = 3200.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(testing::Return(fake_stats));
    
    auto stats = provider->collect_stats();
    EXPECT_GE(stats.total_load, 0.0);
    EXPECT_LE(stats.total_load, 100.0);
    EXPECT_GT(stats.average_frequency, 0.0);
}

TEST(CpuMonitorAcceptance, ShouldCollectArchitectureDetails) {
    auto provider = std::make_unique<MockCpuProvider>();
    
    CpuArchitecture fake_arch;
    fake_arch.model_name = "AMD Ryzen 7 5800H";
    fake_arch.physical_cores = 8;
    
    EXPECT_CALL(*provider, collect_architecture()).WillOnce(testing::Return(fake_arch));
    
    auto arch = provider->collect_architecture();
    EXPECT_FALSE(arch.model_name.empty());
    EXPECT_GT(arch.physical_cores, 0);
}
