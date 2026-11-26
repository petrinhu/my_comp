#include <gtest/gtest.h>
#include "../unit/fakes/in_memory_preferences_store.h"
#include "../../src/presentation/viewmodels/network_viewmodel.h"
#include <QtTest/QSignalSpy>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_network_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_network_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"
#include <thread>
#include <chrono>

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::NetworkViewModel;

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

// QA-03: com tráfego e sem sonda externa (padrão MC-10), o status é "Local Only".
TEST(NetworkMonitorAcceptance, UserSeesLocalStatusWithoutExternalProbe) {
    auto provider = std::make_shared<testing::NiceMock<MockNetworkProvider>>();
    NetworkStats stats;
    stats.total_rx_mbps = 2.5;
    stats.latency_ms = -1.0; // sonda externa desligada
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(stats));

    NetworkViewModel vm(provider, std::make_shared<my_comp::tests::InMemoryPreferencesStore>());
    QSignalSpy changed(&vm, &NetworkViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_EQ(vm.status(), QString("Local Only"));
    EXPECT_FALSE(vm.externalProbeEnabled());
}
