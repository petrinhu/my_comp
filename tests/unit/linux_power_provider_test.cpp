#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_power_provider.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxPowerProviderTest, ShouldCollectPowerStatsFromRealSystem) {
    LinuxPowerProvider provider;
    auto stats = provider.collect_stats();
    
    // Se houver status, provavelmente há bateria
    if (!stats.status.isEmpty()) {
        EXPECT_GE(stats.battery_percent, 0.0);
        EXPECT_LE(stats.battery_percent, 100.0);
        EXPECT_GE(stats.consumption_watts, 0.0);
    } else {
        EXPECT_EQ(stats.battery_percent, 0.0);
    }
}
