#include <gtest/gtest.h>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>
#include "../../src/infrastructure/collectors/systemd_service_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::domain::models::ServiceScope;

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

// SV-01c: --output=json preserva descrições com espaços múltiplos e não quebra se o
// layout de colunas mudar. SV-01b: escopo vira campo, não sufixo no nome.
TEST_F(SystemdServiceProviderTest, ShouldParseListUnitsJson) {
    auto shell = std::make_shared<MockShellExecutor>();
    shell->canned_output = R"([
        {"unit":"dbus.service","load":"loaded","active":"active","sub":"running","description":"D-Bus System Message Bus"},
        {"unit":"sshd.service","load":"loaded","active":"inactive","sub":"dead","description":"OpenSSH   server  daemon"}
    ])";

    SystemdServiceProvider provider(shell);
    auto services = provider.get_services();

    ASSERT_GE(shell->calls.size(), 2u);
    EXPECT_EQ(shell->calls[0].command, "systemctl");
    EXPECT_TRUE(shell->calls[0].args.contains("list-units"));
    EXPECT_TRUE(shell->calls[0].args.contains("--output=json"));

    ASSERT_EQ(services.size(), 4u); // 2 system + 2 user (mock devolve o mesmo JSON)
    EXPECT_EQ(services[0].name, "dbus.service");
    EXPECT_EQ(services[0].activeState, "active");
    EXPECT_EQ(services[0].scope, ServiceScope::System);
    EXPECT_EQ(services[1].description, "OpenSSH   server  daemon");
    EXPECT_EQ(services[2].name, "dbus.service"); // sem " (User)" no nome
    EXPECT_EQ(services[2].scope, ServiceScope::User);
}

// Saída não-JSON (systemd < 246 ou erro) não pode derrubar nem gerar lixo.
TEST_F(SystemdServiceProviderTest, IgnoresMalformedJson) {
    auto shell = std::make_shared<MockShellExecutor>();
    shell->canned_output = "dbus.service loaded active running D-Bus\n";
    SystemdServiceProvider provider(shell);
    EXPECT_TRUE(provider.get_services().empty());
}

TEST_F(SystemdServiceProviderTest, ShouldIssueStartAndStopViaSystemctl) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);

    provider.start_service("foo.service", ServiceScope::System);
    provider.stop_service("bar.service", ServiceScope::User);

    ASSERT_EQ(shell->calls.size(), 2u);
    EXPECT_EQ(shell->calls[0].args, (QStringList{"enable", "--now", "foo.service"}));
    EXPECT_EQ(shell->calls[1].args, (QStringList{"--user", "disable", "--now", "bar.service"}));
}

TEST_F(SystemdServiceProviderTest, ShouldCreateDaemonUnitFileInTestConfigDir) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);

    ASSERT_TRUE(provider.create_daemon("/usr/bin/true", "--flag").ok);

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

    EXPECT_FALSE(provider.create_daemon("/usr/bin/true", "--ok\n[Service]\nExecStart=/bin/evil").ok);
    EXPECT_FALSE(provider.create_daemon("/usr/bin/true\nRestart=no", "--ok").ok);
    EXPECT_FALSE(provider.create_daemon("/usr/bin/true\r\n", "--ok").ok);

    // Nenhuma chamada a systemctl deve ocorrer se a validação barrou.
    EXPECT_TRUE(shell->calls.empty());

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
    EXPECT_FALSE(QFile::exists(configDir + "/true-daemon.service"));
}

namespace {
QString make_executable(const QString& dir, const QString& name, bool exec = true) {
    QDir().mkpath(dir);
    const QString path = dir + "/" + name;
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write("#!/bin/sh\nexit 0\n");
    f.close();
    f.setPermissions(exec ? (QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner)
                          : (QFile::ReadOwner | QFile::WriteOwner));
    return path;
}
QString unit_dir() {
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/systemd/user";
}
} // namespace

// SV-03c: unit apontando para algo que não roda nasce quebrado; recusar antes de gravar,
// com mensagem que diga ao usuário o que corrigir.
TEST_F(SystemdServiceProviderTest, RejectsMissingNonExecutableOrRelativeExe) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());

    const auto missing = provider.create_daemon(tmp.path() + "/nao-existe", "");
    EXPECT_FALSE(missing.ok);
    EXPECT_TRUE(missing.message.contains("não existe")) << missing.message.toStdString();

    const QString plain = make_executable(tmp.path(), "sem-exec", false);
    const auto not_exec = provider.create_daemon(plain, "");
    EXPECT_FALSE(not_exec.ok);
    EXPECT_TRUE(not_exec.message.contains("executável")) << not_exec.message.toStdString();

    const auto relative = provider.create_daemon("true", "");
    EXPECT_FALSE(relative.ok);
    EXPECT_TRUE(relative.message.contains("absoluto")) << relative.message.toStdString();

    const auto dir = provider.create_daemon(tmp.path(), "");
    EXPECT_FALSE(dir.ok);

    EXPECT_TRUE(shell->calls.empty());
}

TEST_F(SystemdServiceProviderTest, SuccessCarriesUnitNameInMessage) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);
    const auto r = provider.create_daemon("/usr/bin/true", "");
    EXPECT_TRUE(r.ok);
    EXPECT_TRUE(r.message.contains("true-daemon.service")) << r.message.toStdString();
}

// SV-03f: dois executáveis com o mesmo nome em pastas diferentes não podem sobrescrever
// o unit um do outro em silêncio.
TEST_F(SystemdServiceProviderTest, SameBaseNameInDifferentDirsDoesNotOverwriteUnit) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString a = make_executable(tmp.path() + "/a", "my_tool");
    const QString b = make_executable(tmp.path() + "/b", "my_tool");

    ASSERT_TRUE(provider.create_daemon(a, "").ok);
    QFile first(unit_dir() + "/my_tool-daemon.service");
    ASSERT_TRUE(first.open(QIODevice::ReadOnly));
    const QString first_content = QString::fromUtf8(first.readAll());
    first.close();

    const auto rb = provider.create_daemon(b, "");
    ASSERT_TRUE(rb.ok);
    ASSERT_TRUE(first.open(QIODevice::ReadOnly));
    EXPECT_EQ(QString::fromUtf8(first.readAll()), first_content); // não sobrescreveu
    EXPECT_FALSE(rb.message.contains("my_tool-daemon.service")) << rb.message.toStdString();

    const QStringList units = QDir(unit_dir()).entryList({"my_tool*-daemon.service"}, QDir::Files);
    EXPECT_EQ(units.size(), 2);
}

TEST_F(SystemdServiceProviderTest, RecreatingSameExeReusesItsUnit) {
    auto shell = std::make_shared<MockShellExecutor>();
    SystemdServiceProvider provider(shell);
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString a = make_executable(tmp.path(), "my_tool");
    ASSERT_TRUE(provider.create_daemon(a, "--v1").ok);
    ASSERT_TRUE(provider.create_daemon(a, "--v2").ok);
    const QStringList units = QDir(unit_dir()).entryList({"my_tool*-daemon.service"}, QDir::Files);
    EXPECT_EQ(units.size(), 1);
}
