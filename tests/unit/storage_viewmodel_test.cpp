#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/presentation/viewmodels/storage_viewmodel.h"
#include "../../src/domain/interfaces/i_storage_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockStorageProvider : public IStorageProvider {
public:
    MOCK_METHOD(StorageStats, collect_stats, (), (override));
};

TEST(StorageViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockStorageProvider>();
    StorageStats stats;
    stats.total_read_mb_s = 150.0;
    stats.total_write_mb_s = 80.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));
    
    StorageViewModel vm(provider);
    vm.refresh();
    
    EXPECT_DOUBLE_EQ(vm.totalReadMb(), 150.0);
    EXPECT_DOUBLE_EQ(vm.totalWriteMb(), 80.0);
}
