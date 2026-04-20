#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/infrastructure/collectors/linux_smart_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(LinuxSmartProviderTest, ShouldParseNvmeWearAndTemperature) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString json = R"({
      "smart_status": {"passed": true},
      "temperature": {"current": 42},
      "power_on_time": {"hours": 5000},
      "nvme_smart_health_information_log": {
        "percentage_used": 7,
        "temperature": 315
      }
    })";
    EXPECT_CALL(*shell, execute(QString("smartctl"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(0)));

    LinuxSmartProvider provider(shell);
    auto r = provider.collect_report("nvme0n1");

    EXPECT_TRUE(r.available);
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.temp_c, 42);
    EXPECT_EQ(r.power_on_hours, 5000);
    EXPECT_EQ(r.wear_pct, 7);
    EXPECT_TRUE(r.error_message.isEmpty());
}

TEST(LinuxSmartProviderTest, ShouldParseSataAttributes) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString json = R"({
      "smart_status": {"passed": true},
      "temperature": {"current": 38},
      "power_on_time": {"hours": 12345},
      "ata_smart_attributes": {
        "table": [
          {"id": 5, "name": "Reallocated_Sector_Ct", "raw": {"value": 0}, "thresh": 10},
          {"id": 194, "name": "Temperature_Celsius", "raw": {"value": 38}, "thresh": 0}
        ]
      }
    })";
    EXPECT_CALL(*shell, execute(QString("smartctl"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(0)));

    LinuxSmartProvider provider(shell);
    auto r = provider.collect_report("sda");

    EXPECT_TRUE(r.available);
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.temp_c, 38);
    EXPECT_EQ(r.power_on_hours, 12345);
    ASSERT_EQ(r.attrs.size(), 2u);
    EXPECT_EQ(r.attrs[0].id, 5);
    EXPECT_EQ(r.attrs[0].name, "Reallocated_Sector_Ct");
    EXPECT_EQ(r.attrs[0].threshold, 10);
    EXPECT_EQ(r.attrs[1].id, 194);
}

TEST(LinuxSmartProviderTest, ShouldCapturePermissionDeniedMessage) {
    auto shell = std::make_shared<MockShellExecutor>();
    // smartctl sem root: rc != 0 e sem smart_status
    const QString json = R"({
      "smartctl": {
        "messages": [
          {"severity": "error", "string": "Permission denied"}
        ]
      }
    })";
    EXPECT_CALL(*shell, execute(QString("smartctl"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(2)));

    LinuxSmartProvider provider(shell);
    auto r = provider.collect_report("sda");

    EXPECT_FALSE(r.available);
    EXPECT_EQ(r.error_message, "Permission denied");
}

TEST(LinuxSmartProviderTest, ShouldReturnErrorOnEmptyOutput) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("smartctl"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("")), Return(127)));

    LinuxSmartProvider provider(shell);
    auto r = provider.collect_report("sda");

    EXPECT_FALSE(r.available);
    EXPECT_FALSE(r.error_message.isEmpty());
}

TEST(LinuxSmartProviderTest, ShouldReturnErrorOnInvalidJson) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("smartctl"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("lixo não JSON")), Return(0)));

    LinuxSmartProvider provider(shell);
    auto r = provider.collect_report("sda");

    EXPECT_FALSE(r.available);
    EXPECT_TRUE(r.error_message.startsWith("JSON inválido"));
}
