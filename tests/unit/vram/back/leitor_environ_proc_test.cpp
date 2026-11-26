#include "domain/vram/valueobjects/pid.h"
#include "infrastructure/collectors/vram/leitor_environ_proc.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using vramopt::domain::PID;
using vramopt::infrastructure::LeitorEnvironProc;

namespace {

void escreverEnviron(const QString& proc_root, int pid,
                     const QList<QPair<QString, QString>>& kvs) {
    const QString dir = QStringLiteral("%1/%2").arg(proc_root).arg(pid);
    QDir{}.mkpath(dir);
    QFile f{dir + QStringLiteral("/environ")};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    for (const auto& kv : kvs) {
        f.write(kv.first.toUtf8());
        f.write("=");
        f.write(kv.second.toUtf8());
        f.write("\0", 1);
    }
}

}

TEST(LeitorEnvironProcTest, ParseiaEntradasSeparadasPorNul) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString proc = tmp.path() + QStringLiteral("/proc");
    escreverEnviron(proc, 1234, {
        {QStringLiteral("DRI_PRIME"), QStringLiteral("1")},
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), QStringLiteral("1")},
        {QStringLiteral("HOME"), QStringLiteral("/home/user")},
    });

    LeitorEnvironProc leitor{proc};
    auto env = leitor.environ_de(PID{1234});

    EXPECT_EQ(env.value(QStringLiteral("DRI_PRIME")), QStringLiteral("1"));
    EXPECT_EQ(env.value(QStringLiteral("__NV_PRIME_RENDER_OFFLOAD")),
              QStringLiteral("1"));
    EXPECT_EQ(env.value(QStringLiteral("HOME")), QStringLiteral("/home/user"));
}

TEST(LeitorEnvironProcTest, ProcInexistenteRetornaVazio) {
    LeitorEnvironProc leitor{QStringLiteral("/tmp/nao_ha_proc_aqui_xyz")};
    EXPECT_TRUE(leitor.environ_de(PID{1}).isEmpty());
}

TEST(LeitorEnvironProcTest, IgnoraEntradasSemSinalDeIgualdade) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString proc = tmp.path() + QStringLiteral("/proc");
    QDir{}.mkpath(proc + QStringLiteral("/77"));
    QFile f{proc + QStringLiteral("/77/environ")};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("ABC", 3);
    f.write("\0", 1);
    f.write("KEY=value", 9);
    f.write("\0", 1);
    f.close();

    LeitorEnvironProc leitor{proc};
    auto env = leitor.environ_de(PID{77});
    EXPECT_FALSE(env.contains(QStringLiteral("ABC")));
    EXPECT_EQ(env.value(QStringLiteral("KEY")), QStringLiteral("value"));
}
