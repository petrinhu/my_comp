#include "infrastructure/collectors/vram/leitor_maps_proc.h"
#include "domain/vram/valueobjects/pid.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <gtest/gtest.h>

#include <unistd.h>

using my_comp::domain::vram::PID;
using my_comp::infrastructure::vram::LeitorMapsProc;

namespace {
void escrever_maps(const QString& root, int pid, const QByteArray& conteudo) {
    const QString dir = QStringLiteral("%1/%2").arg(root).arg(pid);
    QDir{}.mkpath(dir);
    QFile f{dir + QStringLiteral("/maps")};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(conteudo);
}
} // namespace

// /proc/<pid>/maps: "endereço perms offset dev inode [caminho]". Só interessam os
// caminhos de bibliotecas compartilhadas, sem repetição.
TEST(LeitorMapsProcTest, ExtraiBibliotecasUnicasDoMaps) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    escrever_maps(tmp.path(), 42,
        "55d0c0a00000-55d0c0a21000 r--p 00000000 103:02 1234   /usr/bin/python3.12\n"
        "7f1a2b000000-7f1a2b100000 r-xp 00000000 103:02 5678   /usr/lib64/libcudart.so.12.4.127\n"
        "7f1a2b100000-7f1a2b200000 r--p 00100000 103:02 5678   /usr/lib64/libcudart.so.12.4.127\n"
        "7f1a2c000000-7f1a2c010000 rw-p 00000000 00:00 0 \n"
        "7f1a2d000000-7f1a2d001000 r-xp 00000000 103:02 9999   /usr/lib64/libc.so.6\n"
        "7ffd00000000-7ffd00021000 rw-p 00000000 00:00 0      [stack]\n"
        "7f1a2e000000-7f1a2e001000 r-xp 00000000 103:02 777    /opt/app/lib com espaço.so\n");

    LeitorMapsProc leitor{tmp.path()};
    const auto libs = leitor.bibliotecas_mapeadas(PID{42});
    EXPECT_EQ(libs.count(QStringLiteral("/usr/lib64/libcudart.so.12.4.127")), 1);
    EXPECT_TRUE(libs.contains(QStringLiteral("/usr/lib64/libc.so.6")));
    EXPECT_TRUE(libs.contains(QStringLiteral("/opt/app/lib com espaço.so")));
    EXPECT_FALSE(libs.contains(QStringLiteral("/usr/bin/python3.12")));
    EXPECT_FALSE(libs.contains(QStringLiteral("[stack]")));
}

TEST(LeitorMapsProcTest, PidInexistenteDevolveVazio) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    LeitorMapsProc leitor{tmp.path()};
    EXPECT_TRUE(leitor.bibliotecas_mapeadas(PID{999}).isEmpty());
}

// Processo real: o próprio teste mapeia libc/libstdc++.
TEST(LeitorMapsProcTest, LeProcessoAtualEmProc) {
    LeitorMapsProc leitor;
    const auto libs = leitor.bibliotecas_mapeadas(PID{static_cast<std::int32_t>(::getpid())});
    bool tem_so = false;
    for (const auto& l : libs) tem_so = tem_so || l.contains(QStringLiteral(".so"));
    EXPECT_TRUE(tem_so);
}
