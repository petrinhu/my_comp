#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/snap_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(SnapUpdateProviderTest, ShouldIdentifyUpdatesFromSnapOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // snap refresh --list output format:
    // Name    Version  Rev  Publisher  Notes
    // core22  20240111 1122 canonical* -
    QString mock_output = 
        "Name    Version  Rev  Publisher  Notes\n"
        "core22  20240111 1122 canonical* -\n"
        "firefox 121.0-1  3561 mozilla*   -\n";

    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"refresh", "--list"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));
    // IN-07a: segunda chamada `snap list` para enrich_current_versions.
    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"list"}), _))
        .WillRepeatedly(Return(0));

    SnapUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "core22");
    EXPECT_EQ(updates[0].new_version, "20240111");
    EXPECT_EQ(updates[1].name, "firefox");
}

// IN-07b: cobertura ampliada.
TEST(SnapUpdateProviderTest, ShouldEnrichCurrentVersionFromSnapList) {
    auto shell = std::make_shared<MockShellExecutor>();

    QString mock_refresh =
        "Name    Version  Rev  Publisher  Notes\n"
        "firefox 121.0-1  3561 mozilla*   -\n";
    QString mock_list =
        "Name    Version  Rev  Tracking Publisher Notes\n"
        "firefox 120.5-1  3500 latest   mozilla*  -\n";

    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"refresh", "--list"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_refresh), Return(0)));
    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"list"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_list), Return(0)));

    SnapUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].current_version, "120.5-1");
}

TEST(SnapUpdateProviderTest, ShouldReturnEmptyWhenRefreshFails) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"refresh", "--list"}), _))
        .WillOnce(Return(1));

    SnapUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(SnapUpdateProviderTest, IsAvailableReflectsSnapVersionExit) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"--version"}), _))
        .WillOnce(Return(0))
        .WillOnce(Return(127));

    SnapUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
    EXPECT_FALSE(provider.is_available());
}

TEST(SnapUpdateProviderTest, ShouldHandleRefreshOutputWithoutHeader) {
    auto shell = std::make_shared<MockShellExecutor>();
    // Output puro sem cabeçalho — primeira linha é descartada (comportamento documentado).
    QString mock_output = "firefox 121.0-1  3561 mozilla*   -\n";

    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"refresh", "--list"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));
    EXPECT_CALL(*shell, execute(QString("snap"), QStringList({"list"}), _))
        .WillRepeatedly(Return(0));

    SnapUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    // Sem cabeçalho, o provider trata a primeira linha como header e descarta.
    EXPECT_TRUE(updates.empty());
}
