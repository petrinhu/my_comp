#include <gtest/gtest.h>

#include "infrastructure/collectors/proc_cmdline.h"

#include <QFile>
#include <QTemporaryDir>

#include <string>

using my_comp::infrastructure::collectors::format_cmdline;
using my_comp::infrastructure::collectors::read_cmdline;
using namespace std::string_literals;

// AUDIT-30: /proc/<pid>/comm corta em 15 caracteres (TASK_COMM_LEN); cmdline não.

TEST(ProcCmdlineTest, JuntaArgumentosSeparadosPorNul) {
    const auto raw = "./build/acceptance_tests\0--gtest_brief=1\0"s;
    EXPECT_EQ(format_cmdline(raw, QStringLiteral("acceptance_test")),
              QStringLiteral("./build/acceptance_tests --gtest_brief=1"));
}

TEST(ProcCmdlineTest, SemArgumentosNemNulFinal) {
    EXPECT_EQ(format_cmdline("/usr/bin/plasmashell", QStringLiteral("plasmashell")),
              QStringLiteral("/usr/bin/plasmashell"));
}

TEST(ProcCmdlineTest, CmdlineVazioUsaCommDeThreadDoKernel) {
    EXPECT_EQ(format_cmdline("", QStringLiteral("kworker/0:1")), QStringLiteral("kworker/0:1"));
}

TEST(ProcCmdlineTest, SoNulsUsaComm) {
    EXPECT_EQ(format_cmdline("\0\0"s, QStringLiteral("zombie")), QStringLiteral("zombie"));
}

TEST(ProcCmdlineTest, ArgumentosVaziosNoMeioNaoDuplicamEspaco) {
    const auto raw = "prog\0\0arg\0"s;
    EXPECT_EQ(format_cmdline(raw, QStringLiteral("prog")), QStringLiteral("prog arg"));
}

TEST(ProcCmdlineTest, PreservaUtf8) {
    const auto raw = "editor\0relat\xc3\xb3rio.txt"s;
    EXPECT_EQ(format_cmdline(raw, QStringLiteral("editor")), QStringLiteral("editor relatório.txt"));
}

// cmdline pode ter vários MB (classpath de JVM); a leitura é limitada a MAX_CMDLINE_BYTES.
TEST(ProcCmdlineTest, LeituraLimitaCmdlineGigante) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString caminho = dir.filePath(QStringLiteral("cmdline"));
    QFile f{caminho};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("java\0-cp\0", 9);
    f.write(QByteArray(2 * 1024 * 1024, 'x'));
    f.close();

    const QString cmd = read_cmdline(caminho, QStringLiteral("java"));
    EXPECT_TRUE(cmd.startsWith(QStringLiteral("java -cp xxx")));
    EXPECT_LE(cmd.size(), static_cast<qsizetype>(my_comp::infrastructure::collectors::MAX_CMDLINE_BYTES));
}

TEST(ProcCmdlineTest, ArquivoInexistenteUsaComm) {
    EXPECT_EQ(read_cmdline(QStringLiteral("/nao/existe/cmdline"), QStringLiteral("kworker")),
              QStringLiteral("kworker"));
}
