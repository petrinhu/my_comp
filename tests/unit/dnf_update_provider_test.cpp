#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/dnf_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(DnfUpdateProviderTest, ShouldIdentifyUpdatesFromDnfOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    QString mock_output = 
        "kernel.x86_64                     6.13.0-1.fc43         updates\n"
        "qt6-qtbase.x86_64                 6.8.1-2.fc43          updates\n";

    EXPECT_CALL(*shell, execute(QString("dnf"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(100)));

    DnfUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "kernel.x86_64");
    EXPECT_EQ(updates[0].new_version, "6.13.0-1.fc43");
    EXPECT_EQ(updates[1].name, "qt6-qtbase.x86_64");
}
