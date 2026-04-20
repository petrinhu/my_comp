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

// IN-03c: cobertura ampliada.
TEST(FlatpakUpdateProviderTest, ShouldEnrichCurrentVersionFromFlatpakList) {
    auto shell = std::make_shared<MockShellExecutor>();

    QString mock_remote =
        "Firefox\t134.0\torg.mozilla.firefox\n";
    QString mock_list =
        "org.mozilla.firefox\t133.0\n";

    EXPECT_CALL(*shell, execute(QString("flatpak"), Contains(QString("remote-ls")), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_remote), Return(0)));
    EXPECT_CALL(*shell, execute(QString("flatpak"), Not(Contains(QString("remote-ls"))), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_list), Return(0)));

    FlatpakUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].current_version, "133.0");
    EXPECT_EQ(updates[0].source_repo, "org.mozilla.firefox");
}

TEST(FlatpakUpdateProviderTest, ShouldReturnEmptyWhenRemoteLsFails) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("flatpak"), Contains(QString("remote-ls")), _))
        .WillOnce(Return(1));

    FlatpakUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(FlatpakUpdateProviderTest, IsAvailableReflectsWhichFlatpakOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("which"), QStringList({"flatpak"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("/usr/bin/flatpak\n")), Return(0)))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(1)));

    FlatpakUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
    EXPECT_FALSE(provider.is_available());
}

TEST(FlatpakUpdateProviderTest, IgnoresMalformedLines) {
    auto shell = std::make_shared<MockShellExecutor>();
    QString mock_output =
        "linha\tincompleta\n"                                     // 2 colunas só
        "Firefox\t134.0\torg.mozilla.firefox\n";

    EXPECT_CALL(*shell, execute(QString("flatpak"), Contains(QString("remote-ls")), _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(0)));
    EXPECT_CALL(*shell, execute(QString("flatpak"), Not(Contains(QString("remote-ls"))), _))
        .WillRepeatedly(Return(0));

    FlatpakUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].name, "Firefox");
}
