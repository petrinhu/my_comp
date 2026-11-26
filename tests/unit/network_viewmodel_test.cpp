#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/network_viewmodel.h"
#include "../../src/domain/interfaces/i_network_provider.h"
#include "fakes/in_memory_preferences_store.h"

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
    
    NetworkViewModel vm(provider, std::make_shared<my_comp::tests::InMemoryPreferencesStore>());
    QSignalSpy spy(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));

    EXPECT_DOUBLE_EQ(vm.totalRx(), 10.5);
    EXPECT_EQ(vm.status(), "Online");
}

// MC-10: a medição de latência externa é opt-in, desligada por padrão e persistida.
TEST(NetworkViewModelTest, ExternalProbeIsOptInAndForwardedToProvider) {
    auto store = std::make_shared<my_comp::tests::InMemoryPreferencesStore>();
    auto provider = std::make_shared<NiceMock<MockNetworkProvider>>();
    ON_CALL(*provider, collect_stats()).WillByDefault(Return(NetworkStats{}));

    NetworkViewModel vm(provider, store);
    EXPECT_FALSE(vm.externalProbeEnabled());

    QSignalSpy changed(&vm, &NetworkViewModel::externalProbeEnabledChanged);
    EXPECT_CALL(*provider, set_external_probe_enabled(true)).Times(1);
    vm.setExternalProbeEnabled(true);
    EXPECT_TRUE(vm.externalProbeEnabled());
    EXPECT_EQ(changed.count(), 1);
    EXPECT_TRUE(store->value(QStringLiteral("network/external_latency_probe")).toBool());
}

// Preferência salva é aplicada ao provider na 1ª coleta (sem I/O no ctor, MC-04).
TEST(NetworkViewModelTest, SavedOptInIsRestoredOnFirstRefresh) {
    auto store = std::make_shared<my_comp::tests::InMemoryPreferencesStore>();
    store->set_value(QStringLiteral("network/external_latency_probe"), true);
    auto provider = std::make_shared<NiceMock<MockNetworkProvider>>();
    ON_CALL(*provider, collect_stats()).WillByDefault(Return(NetworkStats{}));
    EXPECT_CALL(*provider, set_external_probe_enabled(true)).Times(1);

    NetworkViewModel vm(provider, store);
    QSignalSpy spy(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_TRUE(vm.externalProbeEnabled());
}

TEST(NetworkViewModelTest, StatusIgnoresDisabledLatency) {
    auto provider = std::make_shared<NiceMock<MockNetworkProvider>>();
    NetworkStats stats;
    stats.total_rx_mbps = 1.0;
    stats.latency_ms = -1.0; // desativada
    ON_CALL(*provider, collect_stats()).WillByDefault(Return(stats));

    NetworkViewModel vm(provider, std::make_shared<my_comp::tests::InMemoryPreferencesStore>());
    QSignalSpy spy(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_EQ(vm.status(), "Local Only");
}
