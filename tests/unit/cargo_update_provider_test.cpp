#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/cargo_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(CargoUpdateProviderTest, ShouldIdentifyUpdatesFromCargoOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // Cargo install-update -l output format:
    // Package  Current  Latest
    // clap     4.4.0    4.5.1
    QString mock_output = 
        "Package  Current  Latest\n"
        "clap     4.4.0    4.5.1\n"
        "serde    1.0.180  1.0.190\n";

    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"install-update", "-l"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    CargoUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "clap");
    EXPECT_EQ(updates[0].new_version, "4.5.1");
    EXPECT_EQ(updates[1].name, "serde");
}

// IN-08b: cobertura ampliada.
TEST(CargoUpdateProviderTest, ShouldReturnEmptyWhenCargoFails) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"install-update", "-l"}), _))
        .WillOnce(Return(127));

    CargoUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(CargoUpdateProviderTest, IsAvailableRequiresCargoAndInstallUpdateCrate) {
    auto shell = std::make_shared<MockShellExecutor>();
    // Caso 1: cargo --version OK e subcomando install-update existe → disponível.
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"--version"}), _))
        .WillOnce(Return(0));
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"install-update", "--help"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("Usage: cargo install-update [OPTIONS]")), Return(0)));

    CargoUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
}

TEST(CargoUpdateProviderTest, IsAvailableFailsWhenCargoAbsent) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"--version"}), _))
        .WillOnce(Return(127));

    CargoUpdateProvider provider(shell);
    EXPECT_FALSE(provider.is_available());
}

TEST(CargoUpdateProviderTest, IsAvailableFailsWhenInstallUpdateCrateMissing) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"--version"}), _))
        .WillOnce(Return(0));
    // Sem crate instalada: cargo reclama e não há "install-update" nem "Usage" no stdout.
    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"install-update", "--help"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(101)));

    CargoUpdateProvider provider(shell);
    EXPECT_FALSE(provider.is_available());
}

TEST(CargoUpdateProviderTest, SkipsHeaderAndSeparatorLines) {
    auto shell = std::make_shared<MockShellExecutor>();
    QString mock_output =
        "Package  Current  Latest\n"
        "---------------------\n"
        "clap     4.4.0    4.5.1\n";

    EXPECT_CALL(*shell, execute(QString("cargo"), QStringList({"install-update", "-l"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    CargoUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].name, "clap");
    EXPECT_EQ(updates[0].current_version, "4.4.0");
    EXPECT_EQ(updates[0].new_version, "4.5.1");
}

