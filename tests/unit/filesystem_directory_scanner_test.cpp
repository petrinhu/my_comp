#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <atomic>
#include <memory>
#include "../../src/infrastructure/collectors/filesystem_directory_scanner.h"

using namespace my_comp::infrastructure::collectors;
using namespace my_comp::domain::models;

namespace {

bool write_file(const QString& path, qint64 size) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    QByteArray payload(static_cast<int>(size), 'x');
    f.write(payload);
    f.close();
    return true;
}

} // namespace

TEST(FilesystemDirectoryScannerTest, ShouldAggregateSizesAndCountsRecursively) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    QDir root(tmp.path());
    ASSERT_TRUE(root.mkpath("a/b"));
    ASSERT_TRUE(root.mkpath("c"));

    ASSERT_TRUE(write_file(root.filePath("a/f1.txt"), 100));
    ASSERT_TRUE(write_file(root.filePath("a/b/f2.txt"), 250));
    ASSERT_TRUE(write_file(root.filePath("c/f3.txt"), 50));

    FilesystemDirectoryScanner scanner;
    auto node = scanner.scan(tmp.path(), nullptr, nullptr);

    EXPECT_EQ(node.size_bytes, 400);
    EXPECT_EQ(node.file_count, 3);
    // dir_count inclui subdirs recursivos: a, a/b, c = 3
    EXPECT_EQ(node.dir_count, 3);
    EXPECT_EQ(node.children.size(), 2u);
}

TEST(FilesystemDirectoryScannerTest, ShouldRespectCancelToken) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    QDir root(tmp.path());
    // Cria uma árvore pequena (suficiente para o loop entrar uma vez).
    for (int i = 0; i < 5; ++i) {
        ASSERT_TRUE(root.mkpath(QString("dir%1").arg(i)));
        ASSERT_TRUE(write_file(root.filePath(QString("dir%1/f.txt").arg(i)), 10));
    }

    auto token = std::make_shared<std::atomic<bool>>(true); // cancela antes de começar
    FilesystemDirectoryScanner scanner;
    auto node = scanner.scan(tmp.path(), token, nullptr);

    // Cancelado imediatamente — nada foi contado.
    EXPECT_EQ(node.size_bytes, 0);
    EXPECT_EQ(node.file_count, 0);
}

TEST(FilesystemDirectoryScannerTest, ShouldFlagErrorOnNonexistentRoot) {
    FilesystemDirectoryScanner scanner;
    auto node = scanner.scan("/caminho/que/nao/existe/xyz123", nullptr, nullptr);
    EXPECT_TRUE(node.error);
    EXPECT_EQ(node.size_bytes, 0);
}

TEST(FilesystemDirectoryScannerTest, ShouldSkipKnownPseudoFilesystems) {
    FilesystemDirectoryScanner scanner;
    auto node = scanner.scan("/proc", nullptr, nullptr);
    EXPECT_TRUE(node.error);
    EXPECT_EQ(node.size_bytes, 0);
}

TEST(FilesystemDirectoryScannerTest, ShouldInvokeProgressCallbackAtLeastOnce) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    ASSERT_TRUE(write_file(QDir(tmp.path()).filePath("a.txt"), 10));

    int calls = 0;
    ScanProgress last;
    FilesystemDirectoryScanner scanner;
    scanner.scan(tmp.path(), nullptr, [&](const ScanProgress& p) {
        ++calls;
        last = p;
    });

    // O snapshot final sempre é emitido.
    EXPECT_GE(calls, 1);
    EXPECT_EQ(last.current_path, tmp.path());
}
