#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtTest/QSignalSpy>
#include "../../src/presentation/viewmodels/export_viewmodel.h"
#include "../../src/domain/interfaces/i_export_provider.h"
#include "fakes/in_memory_preferences_store.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;
using my_comp::application::ExportHistory;
using my_comp::tests::InMemoryPreferencesStore;

namespace {
std::shared_ptr<ExportHistory> fresh_history() {
    return std::make_shared<ExportHistory>(std::make_shared<InMemoryPreferencesStore>());
}
} // namespace

class MockExportProvider : public IExportProvider {
public:
    MOCK_METHOD(bool, export_report, (const ReportData&, const QString&), (override));
    MOCK_METHOD(QString, get_format_name, (), (const, override));
};

TEST(ExportViewModelTest, ShouldHandleExport) {
    ExportViewModel vm(fresh_history());
    auto provider = std::make_unique<MockExportProvider>();

    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("Mock"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));

    vm.add_provider(std::move(provider));

    QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
    vm.exportReport(0, "test.mock");
    spy.wait(2000);

    EXPECT_FALSE(spy.isEmpty());
}

// Histórico persiste no IPreferencesStore injetado (aqui em memória; QSettings real tem
// teste próprio em qsettings_preferences_store_test.cpp).
class ExportHistoryTest : public ::testing::Test {
protected:
    std::shared_ptr<InMemoryPreferencesStore> store = std::make_shared<InMemoryPreferencesStore>();
    std::shared_ptr<ExportHistory> shared_history = std::make_shared<ExportHistory>(store);
};

TEST_F(ExportHistoryTest, ShouldStartEmpty) {
    ExportViewModel vm(shared_history);
    EXPECT_TRUE(vm.exportHistory().isEmpty());
}

TEST_F(ExportHistoryTest, ShouldAppendEntryOnSuccessfulExport) {
    ExportViewModel vm(shared_history);
    auto provider = std::make_unique<MockExportProvider>();
    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("JSON"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));
    vm.add_provider(std::move(provider));

    QSignalSpy finished_spy(&vm, &ExportViewModel::exportFinished);
    QSignalSpy history_spy(&vm, &ExportViewModel::exportHistoryChanged);
    vm.exportReport(0, "/tmp/test_report.json");
    ASSERT_TRUE(finished_spy.wait(2000));

    EXPECT_FALSE(history_spy.isEmpty());

    const auto history = vm.exportHistory();
    ASSERT_EQ(history.size(), 1);
    const auto entry = history[0].toMap();
    EXPECT_EQ(entry.value("format").toString(), "JSON");
    EXPECT_EQ(entry.value("path").toString(), "/tmp/test_report.json");
    EXPECT_FALSE(entry.value("timestamp").toString().isEmpty());
}

TEST_F(ExportHistoryTest, ShouldNotAppendOnFailedExport) {
    ExportViewModel vm(shared_history);
    auto provider = std::make_unique<MockExportProvider>();
    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("JSON"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(false));
    vm.add_provider(std::move(provider));

    QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
    vm.exportReport(0, "/tmp/fail_report.json");
    ASSERT_TRUE(spy.wait(2000));

    EXPECT_TRUE(vm.exportHistory().isEmpty());
}

TEST_F(ExportHistoryTest, ShouldPersistAcrossInstances) {
    {
        ExportViewModel vm(shared_history);
        auto provider = std::make_unique<MockExportProvider>();
        EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("Markdown"));
        EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));
        vm.add_provider(std::move(provider));

        QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
        vm.exportReport(0, "/tmp/persisted.md");
        ASSERT_TRUE(spy.wait(2000));
    }

    ExportViewModel vm2(std::make_shared<ExportHistory>(store)); // nova instância, mesmo store
    const auto history = vm2.exportHistory();
    ASSERT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].toMap().value("format").toString(), "Markdown");
}

TEST_F(ExportHistoryTest, ShouldClearHistory) {
    ExportViewModel vm(shared_history);
    auto provider = std::make_unique<MockExportProvider>();
    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("HTML"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));
    vm.add_provider(std::move(provider));

    QSignalSpy finished_spy(&vm, &ExportViewModel::exportFinished);
    vm.exportReport(0, "/tmp/report.html");
    ASSERT_TRUE(finished_spy.wait(2000));
    ASSERT_EQ(vm.exportHistory().size(), 1);

    QSignalSpy changed_spy(&vm, &ExportViewModel::exportHistoryChanged);
    vm.clearHistory();
    EXPECT_TRUE(vm.exportHistory().isEmpty());
    EXPECT_FALSE(changed_spy.isEmpty());
}

TEST_F(ExportHistoryTest, ShouldCapHistoryAtMaximum) {
    // Valida FIFO: semeia 52 entradas no store e verifica que o append subsequente
    // (via export) mantém a lista em 50 (remove as 2 mais antigas).
    QList<QVariantMap> seed;
    for (int i = 0; i < 52; ++i) {
        seed.append(QVariantMap{{"timestamp", QString("2026-01-01T00:00:%1").arg(i, 2, 10, QChar('0'))},
                                {"format", "Seed"},
                                {"path", QString("/seed/%1").arg(i)}});
    }
    store->write_list(ExportHistory::KEY, seed);

    ExportViewModel vm(shared_history);
    auto provider = std::make_unique<MockExportProvider>();
    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("JSON"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));
    vm.add_provider(std::move(provider));

    QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
    vm.exportReport(0, "/tmp/cap_test.json");
    ASSERT_TRUE(spy.wait(2000));

    const auto history = vm.exportHistory();
    EXPECT_EQ(history.size(), 50);
    // Os 2 seeds mais antigos (0 e 1) foram removidos; o mais recente é o novo export.
    EXPECT_EQ(history.first().toMap().value("path").toString(), "/seed/3");
    EXPECT_EQ(history.last().toMap().value("path").toString(), "/tmp/cap_test.json");
}
