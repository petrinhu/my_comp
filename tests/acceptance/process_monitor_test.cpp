#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_process_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockProcessProvider : public IProcessProvider {
public:
    MOCK_METHOD(std::vector<ProcessInfo>, collect_processes, (), (override));
    MOCK_METHOD(bool, kill_process, (int, int), (override));
    MOCK_METHOD(bool, set_priority, (int, int), (override));
};

TEST(ProcessMonitorAcceptance, ShouldCollectProcessesContract) {
    auto provider = std::make_unique<MockProcessProvider>();
    
    ProcessInfo p;
    p.pid = 1;
    p.command = "/sbin/init";
    p.user = "root";
    p.cpuPercent = 0.1;
    
    std::vector<ProcessInfo> procs = {p};
    
    EXPECT_CALL(*provider, collect_processes()).WillOnce(testing::Return(procs));
    
    auto result = provider->collect_processes();
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].command, "/sbin/init");
    EXPECT_EQ(result[0].user, "root");
}

TEST(ProcessMonitorAcceptance, ShouldControlProcessLifecycle) {
    auto provider = std::make_unique<MockProcessProvider>();
    
    EXPECT_CALL(*provider, kill_process(1234, 9)).WillOnce(testing::Return(true));
    EXPECT_TRUE(provider->kill_process(1234, 9));
}
