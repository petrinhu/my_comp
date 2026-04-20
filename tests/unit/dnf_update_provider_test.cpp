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
    // IN-02a: enrich_current_versions dispara uma chamada adicional ao rpm — aceita vazia.
    EXPECT_CALL(*shell, execute(QString("rpm"), _, _))
        .WillRepeatedly(Return(0));

    DnfUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    EXPECT_EQ(updates[0].name, "kernel.x86_64");
    EXPECT_EQ(updates[0].new_version, "6.13.0-1.fc43");
    EXPECT_EQ(updates[1].name, "qt6-qtbase.x86_64");
}

// IN-02c: cobertura ampliada.
TEST(DnfUpdateProviderTest, ShouldEnrichCurrentVersionFromRpmOutput) {
    auto shell = std::make_shared<MockShellExecutor>();

    QString mock_dnf_output =
        "kernel.x86_64                     6.13.0-1.fc43         updates\n";
    QString mock_rpm_output = "kernel 6.12.0-1.fc43\n";

    EXPECT_CALL(*shell, execute(QString("dnf"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_dnf_output), Return(100)));
    EXPECT_CALL(*shell, execute(QString("rpm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_rpm_output), Return(0)));

    DnfUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].current_version, "6.12.0-1.fc43");
    EXPECT_EQ(updates[0].source_repo, "updates");
    EXPECT_TRUE(updates[0].is_critical); // kernel → crítico.
}

TEST(DnfUpdateProviderTest, ShouldReturnEmptyWhenNoUpdates) {
    auto shell = std::make_shared<MockShellExecutor>();
    // Exit 0 (sem updates) — não entra no branch de parsing.
    EXPECT_CALL(*shell, execute(QString("dnf"), _, _))
        .WillOnce(Return(0));

    DnfUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(DnfUpdateProviderTest, IsAvailableReflectsWhichDnfOutput) {
    auto shell = std::make_shared<MockShellExecutor>();

    // `which dnf` retorna caminho → disponível.
    EXPECT_CALL(*shell, execute(QString("which"), QStringList({"dnf"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("/usr/bin/dnf\n")), Return(0)))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(1)));

    DnfUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
    EXPECT_FALSE(provider.is_available());
}

TEST(DnfUpdateProviderTest, IgnoresMalformedLines) {
    auto shell = std::make_shared<MockShellExecutor>();
    QString mock_output =
        "linha-invalida\n"                                        // só 1 coluna
        "qt6-qtbase.x86_64                 6.8.1-2.fc43          updates\n";

    EXPECT_CALL(*shell, execute(QString("dnf"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(100)));
    EXPECT_CALL(*shell, execute(QString("rpm"), _, _))
        .WillRepeatedly(Return(0));

    DnfUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].name, "qt6-qtbase.x86_64");
}
