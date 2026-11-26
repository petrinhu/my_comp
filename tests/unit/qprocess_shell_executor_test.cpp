#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

#include <QElapsedTimer>

using namespace my_comp::infrastructure::collectors;

// O timeout fixo de 5 s derrubava `dnf check-update` (metadados expirados levam mais).
TEST(QProcessShellExecutorTest, TimeoutIsConfigurablePerCall) {
    QProcessShellExecutor shell;
    QString out;
    QElapsedTimer t;
    t.start();
    EXPECT_EQ(shell.execute_with_timeout("sleep", {"3"}, out, 300), -1);
    EXPECT_LT(t.elapsed(), 2500);
}

TEST(QProcessShellExecutorTest, LongerTimeoutLetsCommandFinish) {
    QProcessShellExecutor shell;
    QString out;
    EXPECT_EQ(shell.execute_with_timeout("sh", {"-c", "sleep 0.2; echo ok"}, out, 5000), 0);
    EXPECT_EQ(out.trimmed(), "ok");
}

TEST(QProcessShellExecutorTest, MissingBinaryReturnsError) {
    QProcessShellExecutor shell;
    QString out;
    EXPECT_NE(shell.execute("my_comp-binario-inexistente", {}, out), 0);
}

TEST(ShellTimeoutPolicyTest, LongRunningOperationsGetLongerThanDefault) {
    EXPECT_GT(PACKAGE_MANAGER_TIMEOUT_MS, DEFAULT_SHELL_TIMEOUT_MS);
    EXPECT_GT(BENCHMARK_TIMEOUT_MS, DEFAULT_SHELL_TIMEOUT_MS);
}
