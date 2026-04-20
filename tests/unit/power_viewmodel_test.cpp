#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/presentation/viewmodels/power_viewmodel.h"
#include "../../src/domain/interfaces/i_power_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockPowerProvider : public IPowerProvider {
public:
    MOCK_METHOD(PowerStats, collect_stats, (), (override));
};

TEST(PowerViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockPowerProvider>();
    PowerStats stats;
    stats.consumption_watts = 15.5;
    stats.battery_percent = 85.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));

    PowerViewModel vm(provider);
    // AR-07: ctor sem I/O, initialize() dispara update() e timer.
    vm.initialize();

    EXPECT_DOUBLE_EQ(vm.powerWatts(), 15.5);
    EXPECT_DOUBLE_EQ(vm.batteryPercent(), 85.0);
}
