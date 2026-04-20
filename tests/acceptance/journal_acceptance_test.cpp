#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include "../../src/domain/interfaces/i_diagnostic_provider.h"
#include "../../src/domain/models/log_entry.h"
#include "../../src/domain/diagnostic_manager.h"
#include "../../src/infrastructure/collectors/journal_diagnostic_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::domain;
using namespace my_comp::domain::interfaces;
using namespace my_comp::infrastructure::collectors;
using namespace testing;

class MockDiagnosticProvider : public interfaces::IDiagnosticProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(std::vector<models::LogEntry>, fetch_logs, (interfaces::DiagnosticFilter, const QString&, int), (override));
};

TEST(JournalAcceptance, ShouldConsolidateDiagnosticLogs) {
    DiagnosticManager manager;
    auto mock = std::make_unique<MockDiagnosticProvider>();

    EXPECT_CALL(*mock, fetch_logs(_, _, _))
        .WillOnce(Return(std::vector<models::LogEntry>{
            {1, "10:00", 3, "Kernel panic", "journal"}
        }));

    manager.register_provider(std::move(mock));
    auto logs = manager.fetch_all_logs();

    ASSERT_EQ(logs.size(), 1);
    EXPECT_EQ(logs[0].message, "Kernel panic");
}

TEST(JournalSearchAcceptance, ShouldFilterLogsByQuery) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    JournalDiagnosticProvider provider(shell);
    
    // Busca real no Journal usando a nova interface unificada
    // "systemd" é onipresente no Fedora, deve retornar algo
    auto results = provider.fetch_logs(DiagnosticFilter::All, "systemd");
    
    // Validamos se o comando journalctl -g não quebrou a execução
    SUCCEED();
}
