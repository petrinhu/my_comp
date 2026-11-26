#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_service_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockServiceProvider : public IServiceProvider {
public:
    MOCK_METHOD(std::vector<ServiceInfo>, get_services, (), (override));
    MOCK_METHOD(OperationResult, enable_and_start, (const QString&, ServiceScope), (override));
    MOCK_METHOD(OperationResult, stop, (const QString&, ServiceScope), (override));
    MOCK_METHOD(OperationResult, disable_and_stop, (const QString&, ServiceScope), (override));
    MOCK_METHOD(OperationResult, create_daemon, (const QString&, const QString&), (override));
};

TEST(SystemdServicesAcceptance, ShouldControlServiceLifecycle) {
    auto provider = std::make_unique<MockServiceProvider>();
    
    QString serviceName = "bluetooth.service";
    
    EXPECT_CALL(*provider, enable_and_start(serviceName, ServiceScope::System)).WillOnce(testing::Return(OperationResult{.ok = true, .message = {}}));
    EXPECT_CALL(*provider, stop(serviceName, ServiceScope::System)).WillOnce(testing::Return(OperationResult{.ok = true, .message = {}}));
    EXPECT_CALL(*provider, disable_and_stop(serviceName, ServiceScope::System)).WillOnce(testing::Return(OperationResult{.ok = true, .message = {}}));
    
    EXPECT_TRUE(provider->enable_and_start(serviceName, ServiceScope::System).ok);
    EXPECT_TRUE(provider->stop(serviceName, ServiceScope::System).ok);
    EXPECT_TRUE(provider->disable_and_stop(serviceName, ServiceScope::System).ok);
}

TEST(SystemdServicesAcceptance, ShouldCreateNewDaemon) {
    auto provider = std::make_unique<MockServiceProvider>();
    
    QString exe = "/usr/bin/my-app";
    QString params = "--verbose";
    
    EXPECT_CALL(*provider, create_daemon(exe, params)).WillOnce(testing::Return(OperationResult{.ok = true, .message = {}}));
    
    EXPECT_TRUE(provider->create_daemon(exe, params).ok);
}
