#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include "../../src/infrastructure/collectors/sysfs_gpu_provider.h"

using namespace my_comp::infrastructure::collectors;

namespace {

void write_file(const QString& path, const QByteArray& content) {
    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    f.write(content);
}

} // namespace

TEST(SysfsGpuProviderTest, ShouldParseAmdSysfsLayout) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    QString base = dir.path() + "/card0/device";
    QDir().mkpath(base + "/hwmon/hwmon3");

    write_file(base + "/vendor", "0x1002\n");
    write_file(base + "/gpu_busy_percent", "42\n");
    write_file(base + "/mem_info_vram_total", "8589934592\n");     // 8 GiB
    write_file(base + "/mem_info_vram_used", "2147483648\n");      // 2 GiB
    write_file(base + "/hwmon/hwmon3/temp1_input", "65000\n");     // 65 °C

    SysfsGpuProvider provider(dir.path());
    auto gpus = provider.collect_stats();

    ASSERT_EQ(gpus.size(), 1u);
    EXPECT_EQ(gpus[0].vendor, "AMD");
    EXPECT_EQ(gpus[0].model_name, "card0");
    EXPECT_TRUE(gpus[0].is_active);
    EXPECT_DOUBLE_EQ(gpus[0].load_percent, 42.0);
    EXPECT_EQ(gpus[0].vram_total_mb, 8192u);
    EXPECT_EQ(gpus[0].vram_used_mb, 2048u);
    EXPECT_NEAR(gpus[0].vram_usage_percent, 25.0, 0.01);
    EXPECT_NEAR(gpus[0].temperature, 65.0, 0.01);
}

TEST(SysfsGpuProviderTest, ShouldReportIntelWithoutAmdMetrics) {
    QTemporaryDir dir;
    QString base = dir.path() + "/card1/device";
    QDir().mkpath(base);
    write_file(base + "/vendor", "0x8086\n");

    SysfsGpuProvider provider(dir.path());
    auto gpus = provider.collect_stats();

    ASSERT_EQ(gpus.size(), 1u);
    EXPECT_EQ(gpus[0].vendor, "Intel");
    EXPECT_DOUBLE_EQ(gpus[0].load_percent, 0.0);
    EXPECT_EQ(gpus[0].vram_total_mb, 0u);
    EXPECT_DOUBLE_EQ(gpus[0].temperature, 0.0);
}

TEST(SysfsGpuProviderTest, ShouldSkipNvidiaCards) {
    QTemporaryDir dir;
    QString base = dir.path() + "/card0/device";
    QDir().mkpath(base);
    write_file(base + "/vendor", "0x10de\n");

    SysfsGpuProvider provider(dir.path());
    EXPECT_TRUE(provider.collect_stats().empty());
}

TEST(SysfsGpuProviderTest, ShouldIgnoreConnectorEntries) {
    QTemporaryDir dir;
    QString connector_base = dir.path() + "/card0-HDMI-A-1/device";
    QDir().mkpath(connector_base);
    write_file(connector_base + "/vendor", "0x1002\n");

    SysfsGpuProvider provider(dir.path());
    EXPECT_TRUE(provider.collect_stats().empty());
}

TEST(SysfsGpuProviderTest, ShouldReturnEmptyWhenDrmRootMissing) {
    SysfsGpuProvider provider(QStringLiteral("/nonexistent/path/drm"));
    EXPECT_TRUE(provider.collect_stats().empty());
}
