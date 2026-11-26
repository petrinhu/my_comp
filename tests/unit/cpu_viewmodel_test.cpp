#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/cpu_viewmodel.h"
#include "../../src/domain/interfaces/i_cpu_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockCpuProvider : public ICpuProvider {
public:
    MOCK_METHOD(CpuStats, collect_stats, (), (override));
    MOCK_METHOD(CpuArchitecture, collect_architecture, (), (override));
};

TEST(CpuViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockCpuProvider>();
    
    CpuArchitecture arch;
    arch.model_name = "Test CPU";
    EXPECT_CALL(*provider, collect_architecture()).WillOnce(Return(arch));

    CpuStats stats;
    stats.total_load = 50.0;
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));

    CpuViewModel vm(provider);
    // MC-04 + AR-07: ctor não faz I/O. initialize() lê arquitetura e dispara 1º refresh.
    QSignalSpy spy(&vm, &CpuViewModel::statsChanged);
    vm.initialize();
    ASSERT_TRUE(spy.wait(2000)); // refresh() é async (QtConcurrent + invokeMethod).

    EXPECT_EQ(vm.modelName(), "Test CPU");
    EXPECT_DOUBLE_EQ(vm.totalLoad(), 50.0);
}
