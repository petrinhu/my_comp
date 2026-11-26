#include <gtest/gtest.h>
#include "../../src/presentation/viewmodels/cpu_viewmodel.h"
#include <QtTest/QSignalSpy>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_cpu_provider.h"
#include <memory>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using my_comp::presentation::viewmodels::CpuViewModel;

// Mock para fins de teste de aceitação (ou usaríamos a implementação real se já existisse)
class MockCpuProvider : public ICpuProvider {
public:
    MOCK_METHOD(CpuStats, collect_stats, (), (override));
    MOCK_METHOD(CpuArchitecture, collect_architecture, (), (override));
};

// QA-03: o usuário vê a carga total e por núcleo que o provider mede.
TEST(CpuMonitorAcceptance, UserSeesTotalAndPerCoreLoad) {
    auto provider = std::make_shared<testing::NiceMock<MockCpuProvider>>();
    CpuStats stats;
    stats.total_load = 45.5;
    stats.average_frequency = 3200.0;
    stats.cores = {{0, 40.0, 3100.0}, {1, 51.0, 3300.0}};
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(stats));
    ON_CALL(*provider, collect_architecture()).WillByDefault(testing::Return(CpuArchitecture{}));

    CpuViewModel vm(provider);
    QSignalSpy changed(&vm, &CpuViewModel::statsChanged);
    vm.initialize();
    ASSERT_TRUE(changed.wait(3000));
    EXPECT_DOUBLE_EQ(vm.totalLoad(), 45.5);
    ASSERT_EQ(vm.coreLoads().size(), 2);
    EXPECT_DOUBLE_EQ(vm.coreLoads().at(1).toMap()["load"].toDouble(), 51.0);
}

// QA-03: modelo e núcleos aparecem no cabeçalho da aba CPU.
TEST(CpuMonitorAcceptance, UserSeesProcessorModelAndCores) {
    auto provider = std::make_shared<testing::NiceMock<MockCpuProvider>>();
    CpuArchitecture arch;
    arch.model_name = "AMD Ryzen 7 5800H";
    arch.physical_cores = 8;
    ON_CALL(*provider, collect_architecture()).WillByDefault(testing::Return(arch));
    ON_CALL(*provider, collect_stats()).WillByDefault(testing::Return(CpuStats{}));

    CpuViewModel vm(provider);
    QSignalSpy arch_changed(&vm, &CpuViewModel::architectureChanged);
    vm.initialize();
    if (arch_changed.isEmpty()) {
        ASSERT_TRUE(arch_changed.wait(3000));
    }
    EXPECT_EQ(vm.modelName(), QString("AMD Ryzen 7 5800H"));
    EXPECT_EQ(vm.physicalCores(), 8);
}
