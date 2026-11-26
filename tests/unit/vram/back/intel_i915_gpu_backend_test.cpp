#include "infrastructure/collectors/vram/intel_i915_gpu_backend.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using my_comp::domain::vram::Fabricante;
using my_comp::infrastructure::vram::IntelI915GpuBackend;

namespace {

void escrever_arquivo(const QString& path, const QString& conteudo) {
    QFile f{path};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text)) << path.toStdString();
    f.write(conteudo.toUtf8());
    f.close();
}

QString montar_card_sysfs(const QString& drm_root,
                        int card_n,
                        const QString& vendor,
                        const QString& pci_id,
                        const QString& pci_slot,
                        std::uint64_t lmem_bytes) {
    const QString card_dir = QStringLiteral("%1/card%2").arg(drm_root).arg(card_n);
    const QString device_dir = card_dir + QStringLiteral("/device");
    QDir{}.mkpath(device_dir);
    escrever_arquivo(device_dir + QStringLiteral("/vendor"), vendor + QStringLiteral("\n"));
    const QString uevent = QStringLiteral(
        "DRIVER=i915\nPCI_ID=%1\nPCI_SLOT_NAME=%2\n").arg(pci_id, pci_slot);
    escrever_arquivo(device_dir + QStringLiteral("/uevent"), uevent);
    if (lmem_bytes > 0) {
        escrever_arquivo(device_dir + QStringLiteral("/mem_info_lmem_total"),
                        QString::number(lmem_bytes) + QStringLiteral("\n"));
    }
    return card_dir;
}

void escrever_proc_fdinfo(const QString& proc_root, int pid, const QString& comm,
                        int fd, const QString& pdev,
                        std::uint64_t total_vram_bytes) {
    const QString pid_dir = QStringLiteral("%1/%2").arg(proc_root).arg(pid);
    QDir{}.mkpath(pid_dir + QStringLiteral("/fdinfo"));
    escrever_arquivo(pid_dir + QStringLiteral("/comm"), comm + QStringLiteral("\n"));
    const QString fd_path = QStringLiteral("%1/fdinfo/%2").arg(pid_dir).arg(fd);
    const QString conteudo = QStringLiteral(
        "pos:\t0\nflags:\t02100002\nmnt_id:\t27\n"
        "drm-driver:\ti915\n"
        "drm-pdev:\t%1\n"
        "drm-client-id:\t1\n"
        "drm-total-vram:\t%2\n").arg(pdev).arg(total_vram_bytes);
    escrever_arquivo(fd_path, conteudo);
}

}

TEST(IntelI915GpuBackendTest, EnumeraGpusFiltrandoPorVendor8086) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm = tmp.path() + QStringLiteral("/drm");
    QDir{}.mkpath(drm);

    montar_card_sysfs(drm, 0, QStringLiteral("0x10de"), QStringLiteral("10DE:1234"),
                    QStringLiteral("0000:01:00.0"), 0);
    montar_card_sysfs(drm, 1, QStringLiteral("0x8086"), QStringLiteral("8086:9A49"),
                    QStringLiteral("0000:00:02.0"), 4ULL * 1024 * 1024 * 1024);

    IntelI915GpuBackend backend{drm, tmp.path() + QStringLiteral("/proc")};

    const auto gpus = backend.enumerar_gpus();
    ASSERT_EQ(gpus.size(), 1U);
    EXPECT_EQ(gpus[0].id(), 0);
    EXPECT_EQ(gpus[0].fabricante(), Fabricante::Intel);
    EXPECT_TRUE(gpus[0].modelo().contains(QStringLiteral("9A49")));
    EXPECT_EQ(gpus[0].vram_total().valor(), 4ULL * 1024 * 1024 * 1024);
}

TEST(IntelI915GpuBackendTest, IgnoraEntradasDeConector) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm = tmp.path() + QStringLiteral("/drm");
    QDir{}.mkpath(drm);
    QDir{}.mkpath(drm + QStringLiteral("/card0-HDMI-A-1"));
    montar_card_sysfs(drm, 0, QStringLiteral("0x8086"), QStringLiteral("8086:9A49"),
                    QStringLiteral("0000:00:02.0"), 0);

    IntelI915GpuBackend backend{drm, tmp.path() + QStringLiteral("/proc")};
    EXPECT_EQ(backend.enumerar_gpus().size(), 1U);
}

TEST(IntelI915GpuBackendTest, RetornaProcessosViaFdinfoMatchPorPdev) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm = tmp.path() + QStringLiteral("/drm");
    const QString proc = tmp.path() + QStringLiteral("/proc");
    QDir{}.mkpath(drm);
    QDir{}.mkpath(proc);

    const QString pdev = QStringLiteral("0000:00:02.0");
    montar_card_sysfs(drm, 0, QStringLiteral("0x8086"), QStringLiteral("8086:9A49"),
                    pdev, 4ULL * 1024 * 1024 * 1024);

    escrever_proc_fdinfo(proc, 1234, QStringLiteral("blender"), 5,
                       pdev, 256ULL * 1024 * 1024);
    escrever_proc_fdinfo(proc, 5678, QStringLiteral("kwin_wayland"), 7,
                       QStringLiteral("0000:99:99.0"), 99);

    IntelI915GpuBackend backend{drm, proc};
    const auto procs = backend.enumerar_processos_vram(0);

    ASSERT_EQ(procs.size(), 1U);
    EXPECT_EQ(procs[0].pid().valor(), 1234);
    EXPECT_EQ(procs[0].nome().valor(), QStringLiteral("blender"));
    EXPECT_EQ(procs[0].vram_consumida().valor(), 256ULL * 1024 * 1024);
    EXPECT_EQ(procs[0].gpu_id(), 0);
    ASSERT_EQ(procs[0].chaves().size(), 1U);
    EXPECT_EQ(procs[0].chaves().front().valor(), QStringLiteral("blender"));
}

TEST(IntelI915GpuBackendTest, RetornaVazioParaGpuIdInexistente) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm = tmp.path() + QStringLiteral("/drm");
    QDir{}.mkpath(drm);
    montar_card_sysfs(drm, 0, QStringLiteral("0x8086"), QStringLiteral("8086:9A49"),
                    QStringLiteral("0000:00:02.0"), 0);
    IntelI915GpuBackend backend{drm, tmp.path() + QStringLiteral("/proc")};
    EXPECT_TRUE(backend.enumerar_processos_vram(99).empty());
}

TEST(IntelI915GpuBackendTest, RetornaVazioQuandoDrmRootAusente) {
    IntelI915GpuBackend backend{
        QStringLiteral("/tmp/inexistente_xyz_12345"),
        QStringLiteral("/tmp/inexistente_proc_xyz")};
    EXPECT_TRUE(backend.enumerar_gpus().empty());
    EXPECT_TRUE(backend.enumerar_processos_vram(0).empty());
}

TEST(IntelI915GpuBackendTest, SistemaTemGpuIntelDetectaCorretamente) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString drm_amd = tmp.path() + QStringLiteral("/drm_amd");
    QDir{}.mkpath(drm_amd);
    montar_card_sysfs(drm_amd, 0, QStringLiteral("0x1002"), QStringLiteral("1002:73DF"),
                    QStringLiteral("0000:03:00.0"), 0);
    EXPECT_FALSE(IntelI915GpuBackend::sistema_tem_gpu_intel(drm_amd));

    const QString drm_intel = tmp.path() + QStringLiteral("/drm_intel");
    QDir{}.mkpath(drm_intel);
    montar_card_sysfs(drm_intel, 0, QStringLiteral("0x8086"), QStringLiteral("8086:9A49"),
                    QStringLiteral("0000:00:02.0"), 0);
    EXPECT_TRUE(IntelI915GpuBackend::sistema_tem_gpu_intel(drm_intel));
}
