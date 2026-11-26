#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <QSettings>
#include "../../src/presentation/viewmodels/network_viewmodel.h"
#include "../../src/domain/interfaces/i_network_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockNetworkProvider : public INetworkProvider {
public:
    MOCK_METHOD(NetworkStats, collect_stats, (), (override));
    MOCK_METHOD(void, set_external_probe_enabled, (bool), (override));
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

// MC-10: a medição de latência externa é opt-in, desligada por padrão e persistida.
TEST(NetworkViewModelTest, ExternalProbeIsOptInAndForwardedToProvider) {
    QSettings().remove(QStringLiteral("network/external_latency_probe"));
    auto provider = std::make_shared<NiceMock<MockNetworkProvider>>();
    ON_CALL(*provider, collect_stats()).WillByDefault(Return(NetworkStats{}));

    NetworkViewModel vm(provider);
    EXPECT_FALSE(vm.externalProbeEnabled());

    QSignalSpy changed(&vm, &NetworkViewModel::externalProbeEnabledChanged);
    EXPECT_CALL(*provider, set_external_probe_enabled(true)).Times(1);
    vm.setExternalProbeEnabled(true);
    EXPECT_TRUE(vm.externalProbeEnabled());
    EXPECT_EQ(changed.count(), 1);
    EXPECT_TRUE(QSettings().value(QStringLiteral("network/external_latency_probe")).toBool());

    QSettings().remove(QStringLiteral("network/external_latency_probe"));
}

TEST(NetworkViewModelTest, StatusIgnoresDisabledLatency) {
    auto provider = std::make_shared<NiceMock<MockNetworkProvider>>();
    NetworkStats stats;
    stats.total_rx_mbps = 1.0;
    stats.latency_ms = -1.0; // desativada
    ON_CALL(*provider, collect_stats()).WillByDefault(Return(stats));

    NetworkViewModel vm(provider);
    QSignalSpy spy(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_EQ(vm.status(), "Local Only");
}
