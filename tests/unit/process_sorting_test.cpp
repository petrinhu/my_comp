#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/presentation/viewmodels/process_viewmodel.h"
#include "../../src/domain/interfaces/i_process_provider.h"
#include <QSignalSpy>

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using ::testing::Return;

class MockProcessProvider : public IProcessProvider {
public:
    MOCK_METHOD(std::vector<ProcessInfo>, collect_processes, (), (override));
    MOCK_METHOD(bool, kill_process, (int, std::uint64_t, int), (override));
    MOCK_METHOD(bool, set_priority, (int, int), (override));
};

TEST(ProcessSortingTest, ShouldSortByCpuDescending) {
    auto provider = std::make_shared<MockProcessProvider>();
    ProcessInfo p1; p1.pid = 1; p1.cpuPercent = 10.0;
    ProcessInfo p2; p2.pid = 2; p2.cpuPercent = 50.0;
    ProcessInfo p3; p3.pid = 3; p3.cpuPercent = 25.0;
    
    EXPECT_CALL(*provider, collect_processes()).WillRepeatedly(Return(std::vector<ProcessInfo>{p1, p2, p3}));
    
    ProcessViewModel vm(provider);
    vm.setSortField("cpu");
    vm.setSortAscending(false); // Descending: 50, 25, 10
    
    QSignalSpy spy(&vm, &ProcessViewModel::processesChanged);
    vm.refresh();
    
    // Esperar pelo processamento assíncrono
    EXPECT_TRUE(spy.wait(2000));
    
    auto list = vm.processes();
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list[0].toMap()["pid"].toInt(), 2);
    EXPECT_EQ(list[1].toMap()["pid"].toInt(), 3);
    EXPECT_EQ(list[2].toMap()["pid"].toInt(), 1);
}

TEST(ProcessSortingTest, ShouldSortByCpuAscending) {
    auto provider = std::make_shared<MockProcessProvider>();
    ProcessInfo p1; p1.pid = 1; p1.cpuPercent = 10.0;
    ProcessInfo p2; p2.pid = 2; p2.cpuPercent = 50.0;
    ProcessInfo p3; p3.pid = 3; p3.cpuPercent = 25.0;
    
    EXPECT_CALL(*provider, collect_processes()).WillRepeatedly(Return(std::vector<ProcessInfo>{p1, p2, p3}));
    
    ProcessViewModel vm(provider);
    vm.setSortField("cpu");
    vm.setSortAscending(true); // Ascending: 10, 25, 50
    
    QSignalSpy spy(&vm, &ProcessViewModel::processesChanged);
    vm.refresh();
    
    EXPECT_TRUE(spy.wait(2000));
    
    auto list = vm.processes();
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list[0].toMap()["pid"].toInt(), 1);
    EXPECT_EQ(list[1].toMap()["pid"].toInt(), 3);
    EXPECT_EQ(list[2].toMap()["pid"].toInt(), 2);
}

TEST(ProcessSortingTest, ShouldHandleEqualValuesInSort) {
    auto provider = std::make_shared<MockProcessProvider>();
    ProcessInfo p1; p1.pid = 1; p1.cpuPercent = 10.0;
    ProcessInfo p2; p2.pid = 2; p2.cpuPercent = 10.0;
    
    EXPECT_CALL(*provider, collect_processes()).WillRepeatedly(Return(std::vector<ProcessInfo>{p1, p2}));
    
    ProcessViewModel vm(provider);
    vm.setSortField("cpu");
    vm.setSortAscending(false);
    
    QSignalSpy spy(&vm, &ProcessViewModel::processesChanged);
    vm.refresh();
    
    // Isso deve completar sem crashar (teste da correção de strict weak ordering)
    EXPECT_TRUE(spy.wait(2000));
    EXPECT_EQ(vm.processes().size(), 2);
}
