#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <unistd.h>

using namespace my_comp::infrastructure::collectors;

namespace {
struct NoShell : IShellExecutor {
    int execute(const QString&, const QStringList&, QString&) override { return 1; }
};
void write_field(const QString& dir, const QString& name, const QByteArray& value, bool readable = true) {
    QFile f(dir + "/" + name);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(value);
    f.close();
    if (!readable) f.setPermissions(QFile::WriteOwner);
}
} // namespace

// HW-01b: todo campo ausente virava "Unknown", indistinguível de um valor real.
TEST(LinuxHardwareSmbiosTest, MissingFieldsAreEmptyNotUnknown) {
    QTemporaryDir dmi;
    ASSERT_TRUE(dmi.isValid());
    write_field(dmi.path(), "sys_vendor", "LENOVO\n");
    write_field(dmi.path(), "product_name", "21HM\n");

    LinuxHardwareProvider provider(std::make_shared<NoShell>(), dmi.path());
    const auto topo = provider.collect_topology();
    EXPECT_EQ(topo.smbios.vendor, QString("LENOVO"));
    EXPECT_EQ(topo.smbios.model, QString("21HM"));
    EXPECT_TRUE(topo.smbios.version.isEmpty());   // arquivo não existe
    EXPECT_TRUE(topo.smbios.boardName.isEmpty());
}

// Firmware genérico preenche campos com texto-placeholder do fabricante da placa.
TEST(LinuxHardwareSmbiosTest, OemPlaceholdersCountAsMissing) {
    QTemporaryDir dmi;
    ASSERT_TRUE(dmi.isValid());
    write_field(dmi.path(), "sys_vendor", "To Be Filled By O.E.M.\n");
    write_field(dmi.path(), "product_version", "Default string\n");
    write_field(dmi.path(), "product_sku", "Not Specified\n");
    write_field(dmi.path(), "board_version", "x.x\n");

    LinuxHardwareProvider provider(std::make_shared<NoShell>(), dmi.path());
    const auto s = provider.collect_topology().smbios;
    EXPECT_TRUE(s.vendor.isEmpty());
    EXPECT_TRUE(s.version.isEmpty());
    EXPECT_TRUE(s.sku.isEmpty());
    EXPECT_TRUE(s.boardVersion.isEmpty());
}

// Serial é 0400 root em /sys: existe, mas não é legível sem privilégio.
TEST(LinuxHardwareSmbiosTest, UnreadableFieldIsMarkedRestricted) {
    if (::geteuid() == 0) GTEST_SKIP() << "root lê qualquer arquivo; cenário não reproduzível";
    QTemporaryDir dmi;
    ASSERT_TRUE(dmi.isValid());
    write_field(dmi.path(), "product_serial", "PF4ABCDE\n", /*readable=*/false);
    write_field(dmi.path(), "board_serial", "L1HF00000\n", /*readable=*/false);

    LinuxHardwareProvider provider(std::make_shared<NoShell>(), dmi.path());
    const auto s = provider.collect_topology().smbios;
    EXPECT_TRUE(s.serial.isEmpty());
    EXPECT_TRUE(s.restrictedFields.contains("serial"));
    EXPECT_TRUE(s.restrictedFields.contains("boardSerial"));
    EXPECT_FALSE(s.restrictedFields.contains("vendor"));
}
