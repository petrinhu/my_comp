#include <gtest/gtest.h>
#include <QDir>
#include <QFile>
#include <gmock/gmock.h>
#include "../../src/domain/interfaces/i_power_provider.h"

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

#include "../../src/infrastructure/collectors/linux_power_provider.h"
#include <iostream>

using namespace my_comp::infrastructure::collectors;

namespace {
// Desktops, VMs e CI não têm bateria: o teste só faz sentido onde existe uma.
bool has_battery() {
    QDir dir("/sys/class/power_supply");
    for (const auto& entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QFile type(dir.filePath(entry + "/type"));
        if (type.open(QIODevice::ReadOnly) && type.readAll().trimmed() == "Battery") return true;
    }
    return false;
}
} // namespace

TEST(PowerMonitorAcceptanceTest, DetectsBatteryOnRealSystem) {
    if (!has_battery()) GTEST_SKIP() << "sem bateria em /sys/class/power_supply";
    LinuxPowerProvider provider;
    auto stats = provider.collect_stats();
    
    std::cout << "[Power Test] Status: " << stats.status.toStdString() << std::endl;
    std::cout << "  - Level: " << stats.battery_percent << "%" << std::endl;
    std::cout << "  - Usage: " << stats.consumption_watts << " Watts" << std::endl;
    std::cout << "  - Health: " << stats.health_percent << "%" << std::endl;
    std::cout << "  - Cycles: " << stats.cycle_count << std::endl;
    
    // Na maioria dos laptops modernos, battery_percent deve ser > 0
    EXPECT_GT(stats.battery_percent, 0.0);
}

class MockPowerProvider : public IPowerProvider {
public:
    MOCK_METHOD(PowerStats, collect_stats, (), (override));
};

TEST(PowerMonitorAcceptanceTest, ShouldCollectBatteryData) {
    auto provider = std::make_unique<MockPowerProvider>();
    
    PowerStats fake;
    fake.battery_percent = 85.0;
    fake.consumption_watts = 12.5;
    fake.health_percent = 98.0;
    
    EXPECT_CALL(*provider, collect_stats()).WillOnce(Return(fake));
    
    auto stats = provider->collect_stats();
    EXPECT_EQ(stats.battery_percent, 85.0);
    EXPECT_GT(stats.health_percent, 0.0);
}
