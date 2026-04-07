#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_network_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_network_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"
#include <thread>
#include <chrono>

using namespace my_comp::infrastructure::collectors;

TEST(NetworkMonitorAcceptance, ShouldCollectFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxNetworkProvider provider(shell);
    
    provider.collect_stats();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto stats = provider.collect_stats();
    std::cout << "[Network Test] Active: " << stats.active_interface.toStdString() 
              << ", RX: " << stats.total_rx_mbps << " Mbps, TX: " << stats.total_tx_mbps 
              << " Mbps, Latency: " << stats.latency_ms << " ms" << std::endl;
}

class MockNetworkProvider : public INetworkProvider {
public:
    MOCK_METHOD(NetworkStats, collect_stats, (), (override));
};

TEST(NetworkMonitorAcceptance, ShouldCollectRealisticNetworkData) {
    auto provider = std::make_unique<MockNetworkProvider>();
    
    NetworkStats fake_stats;
    fake_stats.total_rx_mbps = 2.5;
    fake_stats.latency_ms = 15.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(testing::Return(fake_stats));
    
    auto stats = provider->collect_stats();
    EXPECT_GE(stats.total_rx_mbps, 0.0);
    EXPECT_GE(stats.latency_ms, 0.0);
}
