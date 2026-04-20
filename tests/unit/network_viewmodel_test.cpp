#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/network_viewmodel.h"
#include "../../src/domain/interfaces/i_network_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockNetworkProvider : public INetworkProvider {
public:
    MOCK_METHOD(NetworkStats, collect_stats, (), (override));
};

TEST(NetworkViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockNetworkProvider>();
    NetworkStats stats;
    stats.total_rx_mbps = 10.5;
    stats.total_tx_mbps = 2.1;
    stats.latency_ms = 45.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));
    
    NetworkViewModel vm(provider);
    QSignalSpy spy(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));

    EXPECT_DOUBLE_EQ(vm.totalRx(), 10.5);
    EXPECT_EQ(vm.status(), "Online");
}
