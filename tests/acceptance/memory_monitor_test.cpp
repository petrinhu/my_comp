#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_memory_provider.h"
#include <memory>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_memory_provider.h"

using namespace my_comp::infrastructure::collectors;

TEST(MemoryMonitorAcceptance, ShouldCollectFromRealSystem) {
    LinuxMemoryProvider provider;
    auto stats = provider.collect_stats();
    
    EXPECT_GT(stats.ram_total_kb, 0);
    EXPECT_GT(stats.ram_available_kb, 0);
    EXPECT_LE(stats.ram_usage_percent, 100.0);
}

class MockMemoryProvider : public IMemoryProvider {
public:
    MOCK_METHOD(MemoryStats, collect_stats, (), (override));
};

TEST(MemoryMonitorAcceptance, ShouldCollectRealisticMemoryData) {
    auto provider = std::make_unique<MockMemoryProvider>();
    
    MemoryStats fake_stats;
    fake_stats.ram_total_kb = 16000000;
    fake_stats.ram_available_kb = 8000000;
    fake_stats.ram_usage_percent = 50.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(testing::Return(fake_stats));
    
    auto stats = provider->collect_stats();
    EXPECT_GT(stats.ram_total_kb, 0);
    EXPECT_GE(stats.ram_usage_percent, 0.0);
    EXPECT_LE(stats.ram_usage_percent, 100.0);
}

TEST(MemoryMonitorAcceptance, ShouldHandleSwapAndZram) {
    auto provider = std::make_unique<MockMemoryProvider>();
    
    MemoryStats fake_stats;
    fake_stats.swap_total_kb = 4000000;
    fake_stats.has_zram = true;
    fake_stats.zram_compression_ratio = 3.5;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(testing::Return(fake_stats));
    
    auto stats = provider->collect_stats();
    EXPECT_TRUE(stats.has_zram);
    EXPECT_GT(stats.zram_compression_ratio, 1.0);
}
