#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <chrono>
#include <thread>
#include "../../src/presentation/viewmodels/directory_scanner_viewmodel.h"
#include "../../src/domain/interfaces/i_directory_scanner.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class FakeScanner : public IDirectoryScanner {
public:
    explicit FakeScanner(DirNode result) : m_result(std::move(result)) {}

    DirNode scan(const QString&,
                 std::shared_ptr<std::atomic<bool>> cancel_token,
                 ProgressCallback progress_cb) override {
        if (progress_cb) {
            ScanProgress p;
            p.current_path = "/tmp/x";
            p.bytes_scanned = 500;
            p.files_scanned = 2;
            progress_cb(p);
        }
        m_received_token = cancel_token;
        return m_result;
    }

    std::shared_ptr<std::atomic<bool>> m_received_token;

private:
    DirNode m_result;
};

static DirNode make_node(const QString& name, qint64 size, int file_count = 0) {
    DirNode n;
    n.name = name;
    n.path = "/tmp/" + name;
    n.size_bytes = size;
    n.file_count = file_count;
    return n;
}

TEST(DirectoryScannerViewModelTest, ShouldPopulateTopEntriesSortedBySize) {
    DirNode root;
    root.children.push_back(make_node("small", 100));
    root.children.push_back(make_node("big", 5000));
    root.children.push_back(make_node("medium", 500));
    root.size_bytes = 5600;

    auto scanner = std::make_shared<FakeScanner>(root);
    DirectoryScannerViewModel vm(scanner);

    QSignalSpy spy(&vm, &DirectoryScannerViewModel::resultChanged);
    vm.scan("/tmp");

    ASSERT_TRUE(spy.wait(2000));
    // Pode receber múltiplos sinais (inicialização + final). O último deve conter os dados.
    auto top = vm.topEntries();
    ASSERT_EQ(top.size(), 3);
    EXPECT_EQ(top[0].toMap()["name"].toString(), "big");
    EXPECT_EQ(top[1].toMap()["name"].toString(), "medium");
    EXPECT_EQ(top[2].toMap()["name"].toString(), "small");
    EXPECT_EQ(vm.totalSize(), 5600);
    EXPECT_EQ(vm.scannedRoot(), "/tmp");
}

TEST(DirectoryScannerViewModelTest, ShouldCapTopEntriesAt20) {
    DirNode root;
    for (int i = 0; i < 30; ++i) {
        root.children.push_back(make_node(QString("d%1").arg(i), 100 + i));
    }
    auto scanner = std::make_shared<FakeScanner>(root);
    DirectoryScannerViewModel vm(scanner);
    QSignalSpy spy(&vm, &DirectoryScannerViewModel::resultChanged);
    vm.scan("/tmp");
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_EQ(vm.topEntries().size(), 20);
}

TEST(DirectoryScannerViewModelTest, CancelShouldFlipToken) {
    DirNode root;
    auto scanner = std::make_shared<FakeScanner>(root);
    DirectoryScannerViewModel vm(scanner);

    vm.scan("/tmp");
    vm.cancel();

    // Aguarda worker thread terminar.
    QSignalSpy spy(&vm, &DirectoryScannerViewModel::resultChanged);
    spy.wait(2000);

    // O token recebido pelo scanner deveria ter sido marcado.
    if (scanner->m_received_token) {
        EXPECT_TRUE(scanner->m_received_token->load());
    }
}

TEST(DirectoryScannerViewModelTest, ShouldIgnoreReentrantScanWhileBusy) {
    DirNode root;
    root.children.push_back(make_node("only", 10));
    auto scanner = std::make_shared<FakeScanner>(root);
    DirectoryScannerViewModel vm(scanner);

    vm.scan("/primeiro");
    vm.scan("/segundo"); // deve ser ignorado se ainda estiver scanning

    QSignalSpy spy(&vm, &DirectoryScannerViewModel::resultChanged);
    spy.wait(2000);

    // O root armazenado deve ser o do primeiro scan.
    EXPECT_EQ(vm.scannedRoot(), "/primeiro");
}
