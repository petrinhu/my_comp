#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/flatpak_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(FlatpakUpdateProviderTest, ShouldIdentifyUpdatesFromFlatpakOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    // Flatpak output usually tab-separated
    QString mock_output = 
        "GNOME Application Platform\t47\torg.gnome.Platform\n"
        "Firefox\t134.0\torg.mozilla.firefox\n";

    // IN-03a: ambas as chamadas (remote-ls e list) recebem stub. A primeira match =
    // args contém "remote-ls"; a segunda = args contém "list" (sem "--updates").
    EXPECT_CALL(*shell, execute(QString("flatpak"), Contains(QString("remote-ls")), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));
    EXPECT_CALL(*shell, execute(QString("flatpak"), Not(Contains(QString("remote-ls"))), _))
        .WillRepeatedly(Return(0));

    FlatpakUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "GNOME Application Platform");
    EXPECT_EQ(updates[0].new_version, "47");
    EXPECT_EQ(updates[1].name, "Firefox");
}
