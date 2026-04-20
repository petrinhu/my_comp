#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/memory_viewmodel.h"
#include "../../src/domain/interfaces/i_memory_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockMemoryProvider : public IMemoryProvider {
public:
    MOCK_METHOD(MemoryStats, collect_stats, (), (override));
};

TEST(MemoryViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockMemoryProvider>();
    MemoryStats stats;
    stats.ram_total_kb = 16000000;
    stats.ram_used_kb = 8000000;
    stats.ram_usage_percent = 50.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));
    
    MemoryViewModel vm(provider);
    // AR-08: refresh() agora é async via QtConcurrent.
    QSignalSpy spy(&vm, &MemoryViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));

    EXPECT_DOUBLE_EQ(vm.ramUsagePercent(), 50.0);
    EXPECT_FALSE(vm.ramTotal().isEmpty());
}
