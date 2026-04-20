#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <vector>
#include "../../src/domain/models/update_package.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"
#include "../../src/infrastructure/collectors/npm_update_provider.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(NpmUpdateProviderTest, ShouldIdentifyUpdatesFromNpmJsonOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    
    QString mock_output = R"({
        "typescript": {
            "current": "5.0.0",
            "wanted": "5.1.0",
            "latest": "5.1.0",
            "location": "global"
        },
        "eslint": {
            "current": "8.0.0",
            "wanted": "9.0.0",
            "latest": "9.0.0",
            "location": "global"
        }
    })";

    EXPECT_CALL(*shell, execute(QString("npm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(mock_output), Return(1)));

    NpmUpdateProvider provider(shell);
    auto updates = provider.check_updates();

    ASSERT_EQ(updates.size(), 2);
    // Order might depend on JSON map iteration
    bool found_ts = false;
    for(const auto& u : updates) {
        if(u.name == "typescript") {
            EXPECT_EQ(u.new_version, "5.1.0");
            EXPECT_EQ(u.current_version, "5.0.0");
            found_ts = true;
        }
    }
    EXPECT_TRUE(found_ts);
}

// IN-04a: cobertura ampliada.
TEST(NpmUpdateProviderTest, ShouldReturnEmptyOnInvalidJson) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("npm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("not-json")), Return(1)));

    NpmUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(NpmUpdateProviderTest, ShouldReturnEmptyOnEmptyObject) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("npm"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("{}")), Return(0)));

    NpmUpdateProvider provider(shell);
    auto updates = provider.check_updates();
    EXPECT_TRUE(updates.empty());
}

TEST(NpmUpdateProviderTest, IsAvailableReflectsWhichNpmOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("which"), QStringList({"npm"}), _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("/usr/bin/npm\n")), Return(0)))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(1)));

    NpmUpdateProvider provider(shell);
    EXPECT_TRUE(provider.is_available());
    EXPECT_FALSE(provider.is_available());
}
