#include <gtest/gtest.h>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "../../src/infrastructure/collectors/systemd_service_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

namespace {

struct MockShellExecutor : public IShellExecutor {
    struct Call {
        QString command;
        QStringList args;
    };
    std::vector<Call> calls;
    QString canned_output;
    int exit_code = 0;

    int execute(const QString& command, const QStringList& args, QString& output) override {
        calls.push_back({command, args});
        output = canned_output;
        return exit_code;
    }
};

class SystemdServiceProviderTest : public ::testing::Test {
protected:
    void SetUp() override {
        QStandardPaths::setTestModeEnabled(true);
    }
    void TearDown() override {
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
        QDir(configDir).removeRecursively();
        QStandardPaths::setTestModeEnabled(false);
    }
};

} // namespace

TEST_F(SystemdServiceProviderTest, ShouldParseListUnitsOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    shell->canned_output =
        "dbus.service loaded active running D-Bus System Message Bus\n"
        "sshd.service loaded active running OpenSSH server daemon\n";

    SystemdServiceProvider provider(shell);
    auto services = provider.get_services();

    // Provider chama list-units 2x (system + user); mock devolve o mesmo output.
    ASSERT_EQ(shell->calls.size(), 2u);
    EXPECT_EQ(shell->calls[0].command, "systemctl");
    EXPECT_TRUE(shell->calls[0].args.contains("list-units"));

    EXPECT_EQ(services.size(), 4u); // 2 system + 2 user
    EXPECT_EQ(services[0].name, "dbus.service");
    EXPECT_EQ(services[0].activeState, "active");
}

TEST_F(SystemdServiceProviderTest, ShouldIssueStartAndStopViaSystemctl) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);

    provider.start_service("foo.service");
    provider.stop_service("bar.service (User)");

    ASSERT_EQ(shell->calls.size(), 2u);
    EXPECT_EQ(shell->calls[0].args, (QStringList{"enable", "--now", "foo.service"}));
    EXPECT_EQ(shell->calls[1].args, (QStringList{"--user", "disable", "--now", "bar.service"}));
}

TEST_F(SystemdServiceProviderTest, ShouldCreateDaemonUnitFileInTestConfigDir) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);

    ASSERT_TRUE(provider.create_daemon("/usr/bin/true", "--flag"));

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
    QFile unitFile(configDir + "/true-daemon.service");
    ASSERT_TRUE(unitFile.exists());
    ASSERT_TRUE(unitFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = QString::fromUtf8(unitFile.readAll());
    EXPECT_TRUE(content.contains("ExecStart=/usr/bin/true --flag"));
    EXPECT_TRUE(content.contains("[Install]"));

    // daemon-reload + enable --now
    ASSERT_GE(shell->calls.size(), 2u);
    EXPECT_TRUE(shell->calls[0].args.contains("daemon-reload"));
    EXPECT_TRUE(shell->calls[1].args.contains("enable"));
}

TEST_F(SystemdServiceProviderTest, ShouldRejectInjectionInCreateDaemon) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);

    EXPECT_FALSE(provider.create_daemon("/usr/bin/true", "--ok\n[Service]\nExecStart=/bin/evil"));
    EXPECT_FALSE(provider.create_daemon("/usr/bin/true\nRestart=no", "--ok"));
    EXPECT_FALSE(provider.create_daemon("/usr/bin/true\r\n", "--ok"));

    // Nenhuma chamada a systemctl deve ocorrer se a validação barrou.
    EXPECT_TRUE(shell->calls.empty());

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
    EXPECT_FALSE(QFile::exists(configDir + "/true-daemon.service"));
}
