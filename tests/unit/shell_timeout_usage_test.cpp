#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/infrastructure/collectors/dnf_update_provider.h"
#include "../../src/infrastructure/collectors/flatpak_update_provider.h"
#include "../../src/infrastructure/collectors/npm_update_provider.h"
#include "../../src/infrastructure/collectors/snap_update_provider.h"
#include "../../src/infrastructure/collectors/cargo_update_provider.h"
#include "../../src/infrastructure/collectors/hdparm_benchmark_provider.h"

using namespace my_comp::infrastructure::collectors;
using ::testing::_;
using ::testing::Ge;
using ::testing::Return;

namespace {
class TimeoutAwareShell : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
    MOCK_METHOD(int, execute_with_timeout, (const QString&, const QStringList&, QString&, int), (override));
};
} // namespace

// Consultas que vão à rede (metadados de repositório) precisam do timeout longo.
TEST(ShellTimeoutUsageTest, DnfCheckUpdateUsesPackageManagerTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("dnf"), _, _, Ge(PACKAGE_MANAGER_TIMEOUT_MS)))
        .WillOnce(Return(0));
    (void)DnfUpdateProvider(shell).check_updates();
}

TEST(ShellTimeoutUsageTest, FlatpakRemoteLsUsesPackageManagerTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("flatpak"), _, _, Ge(PACKAGE_MANAGER_TIMEOUT_MS)))
        .WillOnce(Return(1));
    (void)FlatpakUpdateProvider(shell).check_updates();
}

TEST(ShellTimeoutUsageTest, NpmOutdatedUsesPackageManagerTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("npm"), _, _, Ge(PACKAGE_MANAGER_TIMEOUT_MS)))
        .WillOnce(Return(0));
    (void)NpmUpdateProvider(shell).check_updates();
}

TEST(ShellTimeoutUsageTest, SnapRefreshListUsesPackageManagerTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("snap"), _, _, Ge(PACKAGE_MANAGER_TIMEOUT_MS)))
        .WillOnce(Return(1));
    (void)SnapUpdateProvider(shell).check_updates();
}

TEST(ShellTimeoutUsageTest, CargoInstallUpdateUsesPackageManagerTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("cargo"), _, _, Ge(PACKAGE_MANAGER_TIMEOUT_MS)))
        .WillOnce(Return(1));
    (void)CargoUpdateProvider(shell).check_updates();
}

// hdparm -tT leva ~6–10 s: com 5 s o benchmark sempre estourava.
TEST(ShellTimeoutUsageTest, HdparmUsesBenchmarkTimeout) {
    auto shell = std::make_shared<TimeoutAwareShell>();
    EXPECT_CALL(*shell, execute_with_timeout(QString("hdparm"), _, _, Ge(BENCHMARK_TIMEOUT_MS)))
        .WillOnce(Return(1));
    (void)HdparmBenchmarkProvider(shell).run("sda");
}
