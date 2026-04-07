#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include "../../src/domain/interfaces/i_diagnostic_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/journal_diagnostic_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::infrastructure::collectors;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString& program, const QStringList& args, QString& output), (override));
};

TEST(JournalHardwareAcceptanceTest, FiltersHardwareFirmwareLogs) {
    auto mock_shell = std::make_shared<MockShellExecutor>();
    
    // Usamos coringas (_) para os argumentos e validamos a lógica interna se necessário, 
    // mas o foco aqui é a integração do filtro com o comando.
    EXPECT_CALL(*mock_shell, execute(_, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("Apr 04 10:00:00 machine fwupd[123]: Updating firmware\n"
                                     "Apr 04 10:00:01 machine smartd[456]: Device: /dev/sda, is OK"),
                    Return(0)));

    auto provider = std::make_unique<JournalDiagnosticProvider>(mock_shell);
    auto logs = provider->fetch_logs(DiagnosticFilter::HardwareFirmware);
    
    ASSERT_EQ(logs.size(), 2);
    EXPECT_TRUE(logs[0].message.contains("fwupd"));
    EXPECT_TRUE(logs[1].message.contains("smartd"));
}

TEST(JournalHardwareAcceptanceTest, FiltersBootLogs) {
    auto mock_shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*mock_shell, execute(_, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("Apr 04 10:00:00 machine systemd-modules-load[1]: Loaded module\n"
                                     "Apr 04 10:00:01 machine systemd-sysctl[2]: Applied sysctl"),
                    Return(0)));

    auto provider = std::make_unique<JournalDiagnosticProvider>(mock_shell);
    auto logs = provider->fetch_logs(DiagnosticFilter::Boot);
    ASSERT_EQ(logs.size(), 2);
}

TEST(JournalHardwareAcceptanceTest, FiltersDesktopEnvironmentLogs) {
    auto mock_shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*mock_shell, execute(_, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("Apr 04 10:00:00 machine kwin_wayland[1]: Desktop started\n"
                                     "Apr 04 10:00:01 machine plasmashell[2]: Taskbar ready"),
                    Return(0)));

    auto provider = std::make_unique<JournalDiagnosticProvider>(mock_shell);
    auto logs = provider->fetch_logs(DiagnosticFilter::DesktopEnvironment);
    ASSERT_EQ(logs.size(), 2);
}

TEST(JournalHardwareAcceptanceTest, FiltersSecurityNetworkLogs) {
    auto mock_shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*mock_shell, execute(_, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("Apr 04 10:00:00 machine firewalld[1]: Rule added\n"
                                     "Apr 04 10:00:01 machine sshd[2]: Accepted password\n"
                                     "Apr 04 10:00:02 machine sudo[3]: petrus : TTY=pts/0"),
                    Return(0)));

    auto provider = std::make_unique<JournalDiagnosticProvider>(mock_shell);
    auto logs = provider->fetch_logs(DiagnosticFilter::SecurityNetwork);
    ASSERT_EQ(logs.size(), 3);
}
