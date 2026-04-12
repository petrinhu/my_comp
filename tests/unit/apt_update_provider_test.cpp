#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/apt_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(AptUpdateProviderTest, ShouldIdentifyUpdatesFromAptOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    QString mock_output = 
        "Listing... Done\n"
        "bash/focal-updates 5.0-6ubuntu1.2 amd64 [upgradable from: 5.0-6ubuntu1.1]\n"
        "libc6/focal-updates 2.31-0ubuntu9.16 amd64 [upgradable from: 2.31-0ubuntu9.14]\n";

    EXPECT_CALL(*shell, execute(QString("apt"), QStringList({"list", "--upgradable"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));

    AptUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "bash");
    EXPECT_EQ(updates[0].new_version, "5.0-6ubuntu1.2");
    EXPECT_EQ(updates[0].current_version, "5.0-6ubuntu1.1");
    EXPECT_EQ(updates[1].name, "libc6");
}

TEST(AptUpdateProviderTest, ShouldReturnEmptyIfAptFails) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    EXPECT_CALL(*shell, execute(QString("apt"), _, _))
        .WillOnce(Return(1));

    AptUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    EXPECT_TRUE(updates.empty());
}

TEST(AptUpdateProviderTest, ShouldCheckAvailability) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    EXPECT_CALL(*shell, execute(QString("apt"), QStringList({"--version"}), _))
        .WillOnce(Return(0));

    AptUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
}
