#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/domain/interfaces/i_service_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;

class MockServiceProvider : public IServiceProvider {
public:
    MOCK_METHOD(std::vector<ServiceInfo>, get_services, (), (override));
    MOCK_METHOD(bool, start_service, (const QString&, ServiceScope), (override));
    MOCK_METHOD(bool, pause_service, (const QString&, ServiceScope), (override));
    MOCK_METHOD(bool, stop_service, (const QString&, ServiceScope), (override));
    MOCK_METHOD(OperationResult, create_daemon, (const QString&, const QString&), (override));
};

TEST(SystemdServicesAcceptance, ShouldControlServiceLifecycle) {
    auto provider = std::make_unique<MockServiceProvider>();
    
    QString serviceName = "bluetooth.service";
    
    EXPECT_CALL(*provider, start_service(serviceName, ServiceScope::System)).WillOnce(testing::Return(true));
    EXPECT_CALL(*provider, pause_service(serviceName, ServiceScope::System)).WillOnce(testing::Return(true));
    EXPECT_CALL(*provider, stop_service(serviceName, ServiceScope::System)).WillOnce(testing::Return(true));
    
    EXPECT_TRUE(provider->start_service(serviceName, ServiceScope::System));
    EXPECT_TRUE(provider->pause_service(serviceName, ServiceScope::System));
    EXPECT_TRUE(provider->stop_service(serviceName, ServiceScope::System));
}

TEST(SystemdServicesAcceptance, ShouldCreateNewDaemon) {
    auto provider = std::make_unique<MockServiceProvider>();
    
    QString exe = "/usr/bin/my-app";
    QString params = "--verbose";
    
    EXPECT_CALL(*provider, create_daemon(exe, params)).WillOnce(testing::Return(OperationResult{.ok = true, .message = {}}));
    
    EXPECT_TRUE(provider->create_daemon(exe, params).ok);
}
