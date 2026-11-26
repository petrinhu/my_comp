#include <gtest/gtest.h>
#include "../../src/presentation/viewmodels/memory_viewmodel.h"
#include <QtTest/QSignalSpy>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_memory_provider.h"
#include <memory>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_memory_provider.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::MemoryViewModel;

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

// QA-03: a aba Memória mostra total legível e o percentual medido.
TEST(MemoryMonitorAcceptance, UserSeesHumanReadableRamUsage) {
    auto provider = std::make_shared<testing::NiceMock<MockMemoryProvider>>();
    MemoryStats stats;
    stats.ram_total_kb = 16ULL * 1024 * 1024;
    stats.ram_used_kb = 8ULL * 1024 * 1024;
    stats.ram_usage_percent = 50.0;
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(stats));

    MemoryViewModel vm(provider);
    QSignalSpy changed(&vm, &MemoryViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_TRUE(vm.ramTotal().contains("GB")) << vm.ramTotal().toStdString();
    EXPECT_TRUE(vm.ramTotal().startsWith("16")) << vm.ramTotal().toStdString();
    EXPECT_DOUBLE_EQ(vm.ramUsagePercent(), 50.0);
}

TEST(MemoryMonitorAcceptance, UserSeesZramStatus) {
    auto provider = std::make_shared<testing::NiceMock<MockMemoryProvider>>();
    MemoryStats stats;
    stats.swap_total_kb = 4ULL * 1024 * 1024;
    stats.has_zram = true;
    stats.zram_compression_ratio = 3.5;
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(stats));

    MemoryViewModel vm(provider);
    QSignalSpy changed(&vm, &MemoryViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_TRUE(vm.hasZram());
    EXPECT_DOUBLE_EQ(vm.zramCompressionRatio(), 3.5);
    EXPECT_TRUE(vm.swapTotal().contains("GB"));
}
