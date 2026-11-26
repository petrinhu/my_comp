#include <gtest/gtest.h>
#include "../../src/presentation/viewmodels/storage_viewmodel.h"
#include <QtTest/QSignalSpy>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_storage_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_storage_provider.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::StorageViewModel;

#include <thread>
#include <chrono>

TEST(StorageMonitorAcceptance, ShouldCollectFromRealSystem) {
    LinuxStorageProvider provider;
    
    // Primeira coleta para inicializar
    provider.collect_stats();
    
    // Aguardar um pouco para ter diferença nos contadores
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto stats = provider.collect_stats();
    std::cout << "[Storage Test] Total Read: " << stats.total_read_mb_s << " MB/s, Write: " << stats.total_write_mb_s << " MB/s" << std::endl;
    for (const auto& drive : stats.drives) {
        std::cout << "  - " << drive.name.toStdString() << ": R=" << drive.read_mb_s << ", W=" << drive.write_mb_s << std::endl;
    }
}

class MockStorageProvider : public IStorageProvider {
public:
    MOCK_METHOD(StorageStats, collect_stats, (), (override));
};

// QA-03: o throughput somado (leitura + escrita) aparece formatado.
TEST(StorageMonitorAcceptance, UserSeesTotalThroughput) {
    auto provider = std::make_shared<testing::NiceMock<MockStorageProvider>>();
    StorageStats stats;
    stats.total_read_mb_s = 1.2;
    stats.total_write_mb_s = 0.5;
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(stats));

    StorageViewModel vm(provider, nullptr, nullptr, nullptr);
    QSignalSpy changed(&vm, &StorageViewModel::statsChanged);
    vm.refresh(); // a aba chama refresh(); initialize() só carrega o inventário
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_TRUE(vm.totalThroughput().contains("1") && vm.totalThroughput().contains("MB/s"))
        << vm.totalThroughput().toStdString();
    EXPECT_DOUBLE_EQ(vm.totalReadMb(), 1.2);
}
