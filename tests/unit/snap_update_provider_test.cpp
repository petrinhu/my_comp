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

    SnapUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "core22");
    EXPECT_EQ(updates[0].new_version, "20240111");
    EXPECT_EQ(updates[1].name, "firefox");
}
