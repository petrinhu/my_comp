#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_network_provider.h"
#include "mock_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::tests;
using testing::_;
using testing::Return;
using testing::SetArgReferee;

TEST(LinuxNetworkProviderTest, ShouldCollectNetworkStatsFromRealSystem) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // Mock para o ping
    EXPECT_CALL(*shell, execute(QStringLiteral("ping"), _, _))
        .WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(QStringLiteral("64 bytes from 1.1.1.1: icmp_seq=1 ttl=57 time=15.2 ms")),
            Return(0)
        ));

    LinuxNetworkProvider provider(shell);
    provider.set_external_probe_enabled(true); // opt-in explícito do usuário
    auto stats = provider.collect_stats();
    
    EXPECT_GE(stats.total_rx_mbps, 0.0);
    EXPECT_GE(stats.total_tx_mbps, 0.0);
    EXPECT_NEAR(stats.latency_ms, 15.2, 0.1);
}

// MC-10 (100% offline): sem opt-in do usuário nenhum pacote sai para a internet.
TEST(LinuxNetworkProviderTest, DoesNotPingExternalHostByDefault) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QStringLiteral("ping"), _, _)).Times(0);

    LinuxNetworkProvider provider(shell);
    auto stats = provider.collect_stats();
    EXPECT_LT(stats.latency_ms, 0.0); // < 0 = medição desativada
}

TEST(LinuxNetworkProviderTest, StopsPingingWhenOptInIsRevoked) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QStringLiteral("ping"), _, _))
        .Times(1)
        .WillOnce(testing::DoAll(
            SetArgReferee<2>(QStringLiteral("64 bytes from 1.1.1.1: icmp_seq=1 ttl=57 time=9.0 ms")),
            Return(0)));

    LinuxNetworkProvider provider(shell);
    provider.set_external_probe_enabled(true);
    provider.collect_stats();
    provider.set_external_probe_enabled(false);
    EXPECT_LT(provider.collect_stats().latency_ms, 0.0);
}
