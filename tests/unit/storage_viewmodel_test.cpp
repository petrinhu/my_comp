#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <QVariantMap>
#include "../../src/presentation/viewmodels/storage_viewmodel.h"
#include "../../src/domain/interfaces/i_storage_provider.h"
#include "../../src/domain/interfaces/i_disk_inventory_provider.h"
#include "../../src/domain/interfaces/i_smart_provider.h"
#include "../../src/domain/interfaces/i_benchmark_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockStorageProvider : public IStorageProvider {
public:
    MOCK_METHOD(StorageStats, collect_stats, (), (override));
};

class MockInventoryProvider : public IDiskInventoryProvider {
public:
    MOCK_METHOD(DiskInventory, collect_inventory, (), (override));
};

class MockSmartProvider : public ISmartProvider {
public:
    MOCK_METHOD(SmartReport, collect_report, (const QString&), (override));
};

class MockBenchmarkProvider : public IBenchmarkProvider {
public:
    MOCK_METHOD(BenchmarkResult, run, (const QString&), (override));
};

TEST(StorageViewModelTest, ShouldRefreshStatsFromProvider) {
    auto provider = std::make_shared<MockStorageProvider>();
    StorageStats stats;
    stats.total_read_mb_s = 150.0;
    stats.total_write_mb_s = 80.0;

    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(stats));

    StorageViewModel vm(provider);
    QSignalSpy spy(&vm, &StorageViewModel::statsChanged);
    vm.refresh();
    ASSERT_TRUE(spy.wait(2000));

    EXPECT_DOUBLE_EQ(vm.totalReadMb(), 150.0);
    EXPECT_DOUBLE_EQ(vm.totalWriteMb(), 80.0);
}

TEST(StorageViewModelTest, ShouldPopulateDrivesFromInventoryAndSmart) {
    auto provider = std::make_shared<MockStorageProvider>();
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(StorageStats{}));

    auto inventory = std::make_shared<MockInventoryProvider>();
    DiskInventory inv;
    DiskDrive d;
    d.name = "sda";
    d.size_bytes = 1000;
    d.vendor = "Seagate";
    d.model = "ST1000";
    d.rotational = true;
    Partition p;
    p.name = "sda1";
    p.size_bytes = 900;
    p.fs = "ext4";
    p.mount = "/";
    d.partitions.push_back(p);
    inv.drives.push_back(d);
    EXPECT_CALL(*inventory, collect_inventory()).WillOnce(Return(inv));

    auto smart = std::make_shared<MockSmartProvider>();
    SmartReport r;
    r.available = true;
    r.passed = true;
    r.temp_c = 40;
    r.power_on_hours = 1234;
    r.wear_pct = 5;
    EXPECT_CALL(*smart, collect_report(QString("sda"))).WillOnce(Return(r));

    StorageViewModel vm(provider, inventory, smart);
    QSignalSpy spy(&vm, &StorageViewModel::inventoryChanged);
    vm.refreshInventory();
    ASSERT_TRUE(spy.wait(2000));

    const auto drives = vm.drives();
    ASSERT_EQ(drives.size(), 1);
    const auto drive = drives[0].toMap();
    EXPECT_EQ(drive["name"].toString(), "sda");
    EXPECT_EQ(drive["vendor"].toString(), "Seagate");
    EXPECT_EQ(drive["rotational"].toBool(), true);

    const auto smart_map = drive["smart"].toMap();
    EXPECT_TRUE(smart_map["available"].toBool());
    EXPECT_TRUE(smart_map["passed"].toBool());
    EXPECT_EQ(smart_map["tempC"].toInt(), 40);
    EXPECT_EQ(smart_map["wearPct"].toInt(), 5);

    const auto parts = drive["partitions"].toList();
    ASSERT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0].toMap()["name"].toString(), "sda1");
    EXPECT_EQ(parts[0].toMap()["fs"].toString(), "ext4");
}

TEST(StorageViewModelTest, ShouldSkipSmartSubmapWhenProviderAbsent) {
    auto provider = std::make_shared<MockStorageProvider>();
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(StorageStats{}));

    auto inventory = std::make_shared<MockInventoryProvider>();
    DiskInventory inv;
    DiskDrive d;
    d.name = "nvme0n1";
    d.size_bytes = 500;
    inv.drives.push_back(d);
    EXPECT_CALL(*inventory, collect_inventory()).WillOnce(Return(inv));

    StorageViewModel vm(provider, inventory);
    QSignalSpy spy(&vm, &StorageViewModel::inventoryChanged);
    vm.refreshInventory();
    ASSERT_TRUE(spy.wait(2000));

    const auto drives = vm.drives();
    ASSERT_EQ(drives.size(), 1);
    const auto smart_map = drives[0].toMap()["smart"].toMap();
    EXPECT_FALSE(smart_map["available"].toBool());
}

TEST(StorageViewModelTest, ShouldExposeBenchmarkResult) {
    auto provider = std::make_shared<MockStorageProvider>();
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(StorageStats{}));

    auto bench = std::make_shared<MockBenchmarkProvider>();
    BenchmarkResult r;
    r.device = "sda";
    r.seq_read_mb_s = 166.11;
    r.cached_mb_s = 12005.34;
    r.duration_s = 3.2;
    r.success = true;
    EXPECT_CALL(*bench, run(QString("sda"))).WillOnce(Return(r));

    StorageViewModel vm(provider, nullptr, nullptr, bench);
    QSignalSpy spy(&vm, &StorageViewModel::benchmarkChanged);
    vm.runBenchmark("sda");

    // Primeiro sinal: estado "running". Aguarda até receber o resultado final (running=false).
    bool got_final = false;
    for (int i = 0; i < 20 && !got_final; ++i) {
        spy.wait(200);
        if (!vm.isBenchmarking()) got_final = true;
    }
    ASSERT_TRUE(got_final);

    const auto m = vm.lastBenchmark();
    EXPECT_EQ(m["device"].toString(), "sda");
    EXPECT_DOUBLE_EQ(m["seqReadMbS"].toDouble(), 166.11);
    EXPECT_DOUBLE_EQ(m["cachedMbS"].toDouble(), 12005.34);
    EXPECT_TRUE(m["success"].toBool());
    EXPECT_FALSE(m["running"].toBool());
}

TEST(StorageViewModelTest, ShouldIgnoreBenchmarkWhenProviderAbsent) {
    auto provider = std::make_shared<MockStorageProvider>();
    EXPECT_CALL(*provider, collect_stats()).WillRepeatedly(Return(StorageStats{}));
    StorageViewModel vm(provider);
    vm.runBenchmark("sda");
    // Nenhum benchmark registrado, nenhuma mudança emitida.
    EXPECT_FALSE(vm.isBenchmarking());
    EXPECT_TRUE(vm.lastBenchmark().isEmpty());
}
