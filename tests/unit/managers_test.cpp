#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/domain/update_manager.h"
#include "../../src/domain/diagnostic_manager.h"
#include "../../src/domain/interfaces/i_update_provider.h"
#include "../../src/domain/interfaces/i_diagnostic_provider.h"

using namespace my_comp::domain;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockUpdateProvider : public IUpdateProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(bool, is_available, (), (const, override));
    MOCK_METHOD(std::vector<UpdatePackage>, check_updates, (), (override));
};

class MockDiagnosticProvider : public IDiagnosticProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(std::vector<LogEntry>, fetch_logs, (DiagnosticFilter, const QString&, int), (override));
};

TEST(UpdateManagerTest, ShouldCollectFromProviders) {
    UpdateManager manager;
    auto provider = std::make_unique<MockUpdateProvider>();
    
    UpdatePackage p; p.name = "pkg1";
    EXPECT_CALL(*provider, is_available()).WillOnce(Return(true));
    EXPECT_CALL(*provider, check_updates()).WillOnce(Return(std::vector<UpdatePackage>{p}));
    
    manager.register_provider(std::move(provider));
    auto updates = manager.check_all_updates();
    EXPECT_EQ(updates.size(), 1);
}

TEST(DiagnosticManagerTest, ShouldCollectFromProviders) {
    DiagnosticManager manager;
    auto provider = std::make_unique<MockDiagnosticProvider>();
    
    LogEntry log; log.message = "error";
    EXPECT_CALL(*provider, fetch_logs(_, _, _)).WillOnce(Return(std::vector<LogEntry>{log}));
    
    manager.register_provider(std::move(provider));
    auto logs = manager.fetch_all_logs(DiagnosticFilter::All, "");
    EXPECT_EQ(logs.size(), 1);
}
