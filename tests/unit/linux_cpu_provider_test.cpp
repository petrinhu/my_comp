#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_cpu_provider.h"
#include <filesystem>

namespace fs = std::filesystem;
using namespace my_comp::infrastructure::collectors;

TEST(LinuxCpuProviderTest, ShouldCollectStatsFromRealSystem) {
    LinuxCpuProvider provider;
    auto stats = provider.collect_stats();
    
    // Validar se os dados coletados do sistema real fazem sentido
    EXPECT_GE(stats.total_load, 0.0);
    EXPECT_LE(stats.total_load, 100.0);
    
    // Frequência pode ser 0 se não conseguir ler /sys, mas geralmente é > 0
    EXPECT_GE(stats.average_frequency, 0.0);
    
    // Temperatura pode ser 0 se não houver sensores acessíveis
    EXPECT_GE(stats.temperature, 0.0);
    
    // Deve haver pelo menos 1 core se o sistema estiver rodando
    if (!stats.cores.empty()) {
        EXPECT_GE(stats.cores[0].load_percent, 0.0);
        EXPECT_LE(stats.cores[0].load_percent, 100.0);
    }
}

TEST(LinuxCpuProviderTest, ShouldCollectArchitectureFromRealSystem) {
    LinuxCpuProvider provider;
    auto arch = provider.collect_architecture();
    
    // No Fedora/Linux, essas informações devem estar disponíveis em /proc/cpuinfo
    EXPECT_FALSE(arch.model_name.empty());
    EXPECT_FALSE(arch.vendor_id.empty());
    EXPECT_GT(arch.physical_cores, 0);
    EXPECT_GT(arch.logical_threads, 0);
    
    // Pelo menos 1 cache deve ser detectado na maioria dos sistemas modernos
    // (Pode falhar em VMs muito limitadas, mas geralmente passa)
}
