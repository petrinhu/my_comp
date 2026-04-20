#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/infrastructure/collectors/linux_disk_inventory_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;
using namespace testing;

class MockShellExecutor : public IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
};

TEST(LinuxDiskInventoryProviderTest, ShouldParseSingleDiskWithPartitions) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString json = R"({
      "blockdevices": [
        {
          "name": "sda", "size": 1000000000, "type": "disk",
          "vendor": "Seagate", "model": "ST1000", "rota": true,
          "mountpoint": null, "fstype": null, "label": null, "uuid": null,
          "children": [
            {"name": "sda1", "size": 500000000, "type": "part",
             "vendor": null, "model": null, "rota": true,
             "mountpoint": "/nonexistent-mount-xyz", "fstype": "ext4",
             "label": "root", "uuid": "abcd"}
          ]
        }
      ]
    })";

    EXPECT_CALL(*shell, execute(QString("lsblk"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(0)));

    LinuxDiskInventoryProvider provider(shell);
    auto inv = provider.collect_inventory();

    ASSERT_EQ(inv.drives.size(), 1u);
    EXPECT_EQ(inv.drives[0].name, "sda");
    EXPECT_EQ(inv.drives[0].size_bytes, 1000000000);
    EXPECT_EQ(inv.drives[0].vendor, "Seagate");
    EXPECT_EQ(inv.drives[0].model, "ST1000");
    EXPECT_TRUE(inv.drives[0].rotational);

    ASSERT_EQ(inv.drives[0].partitions.size(), 1u);
    EXPECT_EQ(inv.drives[0].partitions[0].name, "sda1");
    EXPECT_EQ(inv.drives[0].partitions[0].fs, "ext4");
    EXPECT_EQ(inv.drives[0].partitions[0].label, "root");
}

TEST(LinuxDiskInventoryProviderTest, ShouldFlattenLuksAndLvmNestedChildren) {
    auto shell = std::make_shared<MockShellExecutor>();
    // nvme0n1 → part(crypto_LUKS) → crypt(lvm member) → lvm(root)
    const QString json = R"({
      "blockdevices": [
        {
          "name": "nvme0n1", "size": 2000, "type": "disk",
          "vendor": null, "model": "NVMe", "rota": false,
          "mountpoint": null, "fstype": null, "label": null, "uuid": null,
          "children": [
            {"name": "nvme0n1p1", "size": 1800, "type": "part",
             "mountpoint": null, "fstype": "crypto_LUKS", "label": null, "uuid": "u1",
             "children": [
               {"name": "luks-root", "size": 1800, "type": "crypt",
                "mountpoint": null, "fstype": "LVM2_member", "label": null, "uuid": "u2",
                "children": [
                  {"name": "vg-root", "size": 1500, "type": "lvm",
                   "mountpoint": "/nonexistent-lvm-mount", "fstype": "ext4",
                   "label": "root", "uuid": "u3"}
                ]}
             ]}
          ]
        }
      ]
    })";
    EXPECT_CALL(*shell, execute(QString("lsblk"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(0)));

    LinuxDiskInventoryProvider provider(shell);
    auto inv = provider.collect_inventory();

    ASSERT_EQ(inv.drives.size(), 1u);
    // 3 entradas achatadas: part, crypt, lvm
    ASSERT_EQ(inv.drives[0].partitions.size(), 3u);
    EXPECT_EQ(inv.drives[0].partitions[0].name, "nvme0n1p1");
    EXPECT_EQ(inv.drives[0].partitions[1].name, "luks-root");
    EXPECT_EQ(inv.drives[0].partitions[2].name, "vg-root");
}

TEST(LinuxDiskInventoryProviderTest, ShouldSkipZramAndNonDiskTopLevel) {
    auto shell = std::make_shared<MockShellExecutor>();
    const QString json = R"({
      "blockdevices": [
        {"name": "zram0", "size": 100, "type": "disk", "rota": false,
         "mountpoint": "[SWAP]", "fstype": "swap", "label": "zram0", "uuid": "z"},
        {"name": "loop0", "size": 100, "type": "loop", "rota": false,
         "mountpoint": null, "fstype": null, "label": null, "uuid": null},
        {"name": "sda", "size": 500, "type": "disk", "rota": true,
         "mountpoint": null, "fstype": null, "label": null, "uuid": null}
      ]
    })";
    EXPECT_CALL(*shell, execute(QString("lsblk"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(json), Return(0)));

    LinuxDiskInventoryProvider provider(shell);
    auto inv = provider.collect_inventory();

    // zram0 passa (type=disk), loop0 não (type=loop). sda passa.
    ASSERT_EQ(inv.drives.size(), 2u);
    EXPECT_EQ(inv.drives[0].name, "zram0");
    EXPECT_EQ(inv.drives[1].name, "sda");
}

TEST(LinuxDiskInventoryProviderTest, ShouldReturnEmptyOnShellFailure) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("lsblk"), _, _))
        .WillOnce(Return(127));  // command not found

    LinuxDiskInventoryProvider provider(shell);
    auto inv = provider.collect_inventory();

    EXPECT_TRUE(inv.drives.empty());
}

TEST(LinuxDiskInventoryProviderTest, ShouldReturnEmptyOnInvalidJson) {
    auto shell = std::make_shared<MockShellExecutor>();
    EXPECT_CALL(*shell, execute(QString("lsblk"), _, _))
        .WillOnce(DoAll(SetArgReferee<2>(QString("not json at all")), Return(0)));

    LinuxDiskInventoryProvider provider(shell);
    auto inv = provider.collect_inventory();

    EXPECT_TRUE(inv.drives.empty());
}
