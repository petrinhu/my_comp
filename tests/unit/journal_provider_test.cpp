#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/log_entry.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/journal_diagnostic_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString& program, const QStringList& args, QString& output), (override));
};

TEST(JournalProviderTest, ShouldParseJournalErrorOutput) {
    auto mock_shell = std::make_shared<MockShellExecutor>();
    
    // Usamos coringas (_) para os matchers de QString
    EXPECT_CALL(*mock_shell, execute(_, _, _))
        .WillOnce(DoAll(SetArgReferee<2>("Apr 03 12:00:00 machine kernel: [error] CPU overheated\n"
                                         "Apr 03 12:00:05 machine kernel: [error] Disk failure"),
                        Return(0)));

    JournalDiagnosticProvider provider(mock_shell);
    auto logs = provider.fetch_logs(DiagnosticFilter::CriticalErrors);

    ASSERT_EQ(logs.size(), 2);
    EXPECT_EQ(logs[0].message, "Apr 03 12:00:00 machine kernel: [error] CPU overheated");
    EXPECT_EQ(logs[1].source, "journal");
}
