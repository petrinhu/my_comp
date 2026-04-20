#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/pacman_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(PacmanUpdateProviderTest, ShouldIdentifyUpdatesFromPacmanOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // pacman -Qu output:
    // bash 5.1.008-1 -> 5.1.016-1
    QString mock_output = 
        "bash 5.1.008-1 -> 5.1.016-1\n"
        "linux 5.15.2.arch1-1 -> 5.15.5.arch1-1\n";

    EXPECT_CALL(*shell, execute(QString("pacman"), QStringList({"-Qu"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    PacmanUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "bash");
    EXPECT_EQ(updates[0].new_version, "5.1.016-1");
    EXPECT_EQ(updates[1].name, "linux");
}

// IN-06a: cobertura ampliada.
TEST(PacmanUpdateProviderTest, ShouldPopulateCurrentVersionFromQuOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    QString mock_output = "bash 5.1.008-1 -> 5.1.016-1\n";

    EXPECT_CALL(*shell, execute(QString("pacman"), QStringList({"-Qu"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    PacmanUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].current_version, "5.1.008-1");
    EXPECT_EQ(updates[0].new_version, "5.1.016-1");
}

TEST(PacmanUpdateProviderTest, ShouldReturnEmptyWhenPacmanFails) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("pacman"), QStringList({"-Qu"}), _))
        .WillOnce(Return(1));

    PacmanUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(PacmanUpdateProviderTest, IsAvailableReflectsPacmanVersionExit) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("pacman"), QStringList({"--version"}), _))
        .WillOnce(Return(0))
        .WillOnce(Return(127));

    PacmanUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
    EXPECT_FALSE(provider.is_available());
}

TEST(PacmanUpdateProviderTest, IgnoresLinesWithoutArrow) {
    auto shell = std::make_shared<MockShellExecutor>();
    QString mock_output =
        "bash 5.1.008-1 5.1.016-1\n"                              // sem "->"
        "linux 5.15.2.arch1-1 -> 5.15.5.arch1-1\n";

    EXPECT_CALL(*shell, execute(QString("pacman"), QStringList({"-Qu"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    PacmanUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].name, "linux");
}
