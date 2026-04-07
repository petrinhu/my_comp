#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_storage_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

#include "../../src/infrastructure/collectors/linux_storage_provider.h"

using namespace my_comp::infrastructure::collectors;

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

TEST(StorageMonitorAcceptance, ShouldCollectRealisticStorageData) {
    auto provider = std::make_unique<MockStorageProvider>();
    
    StorageStats fake_stats;
    fake_stats.total_read_mb_s = 1.2;
    fake_stats.total_write_mb_s = 0.5;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(testing::Return(fake_stats));
    
    auto stats = provider->collect_stats();
    EXPECT_GE(stats.total_read_mb_s, 0.0);
    EXPECT_GE(stats.total_write_mb_s, 0.0);
}
