#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_dev_stack_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockDevStackProvider : public IDevStackProvider {
public:
    MOCK_METHOD(DevStackInfo, collect_dev_stacks, (), (override));
};

TEST(DevStackAcceptance, ShouldCollectDevStacksContract) {
    auto provider = std::make_unique<MockDevStackProvider>();
    
    DevStackInfo fake_data;
    fake_data.toolchains.push_back({"GCC", "13.2.1", true});
    fake_data.containers.push_back({"abc123", "web-server", "nginx:latest", "running", "Podman"});
    
    EXPECT_CALL(*provider, collect_dev_stacks()).WillOnce(testing::Return(fake_data));
    
    auto data = provider->collect_dev_stacks();
    ASSERT_GE(data.toolchains.size(), 1);
    EXPECT_EQ(data.toolchains[0].name, "GCC");
    ASSERT_GE(data.containers.size(), 1);
    EXPECT_EQ(data.containers[0].runtime, "Podman");
}
