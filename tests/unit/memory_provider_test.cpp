#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_memory_provider.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxMemoryProviderTest, ShouldParseMeminfo) {
    LinuxMemoryProvider provider;
    auto stats = provider.collect_stats();
    
    EXPECT_GT(stats.ram_total_kb, 0);
    EXPECT_GT(stats.ram_available_kb, 0);
    EXPECT_GE(stats.ram_total_kb, stats.ram_available_kb);
    EXPECT_GT(stats.ram_usage_percent, 0.0);
    EXPECT_LE(stats.ram_usage_percent, 100.0);
}

TEST(LinuxMemoryProviderTest, ShouldHandleSwap) {
    LinuxMemoryProvider provider;
    auto stats = provider.collect_stats();
    
    // Nem todo sistema tem swap, mas vamos verificar se os campos são consistentes se houver
    if (stats.swap_total_kb > 0) {
        EXPECT_GE(stats.swap_total_kb, stats.swap_free_kb);
        EXPECT_GE(stats.swap_usage_percent, 0.0);
        EXPECT_LE(stats.swap_usage_percent, 100.0);
    }
}
