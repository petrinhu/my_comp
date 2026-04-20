#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/presentation/viewmodels/gpu_viewmodel.h"
#include "../../src/domain/interfaces/i_gpu_provider.h"
#include <QVariantMap>

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockGpuProvider : public IGpuProvider {
public:
    MOCK_METHOD(std::vector<GpuStats>, collect_stats, (), (override));
};

TEST(GpuViewModelTest, ShouldRefreshStatsFromProviders) {
    auto provider = std::make_unique<MockGpuProvider>();
    
    GpuStats stats;
    stats.model_name = "Mock RTX 4090";
    stats.vendor = "NVIDIA";
    stats.load_percent = 10.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(std::vector<GpuStats>{stats}));

    std::vector<std::unique_ptr<IGpuProvider>> providers;
    providers.push_back(std::move(provider));

    GpuViewModel vm(std::move(providers));
    // AR-07: ctor sem I/O; initialize() chama update_stats() e arma o timer.
    vm.initialize();

    auto list = vm.gpus();
    EXPECT_EQ(list.size(), 1);
    
    auto map = list[0].toMap();
    EXPECT_EQ(map["modelName"].toString(), "Mock RTX 4090");
}
