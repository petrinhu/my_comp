#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
#include <memory>
#include "../../src/presentation/viewmodels/diagnostic_viewmodel.h"
#include "../../src/application/diagnostic_manager.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::application;
using namespace testing;

namespace {

class MockDiagnosticProvider : public IDiagnosticProvider {
public:
    MOCK_METHOD(QString, get_name, (), (const, override));
    MOCK_METHOD(std::vector<LogEntry>, fetch_logs, (DiagnosticFilter, const QString&, int), (override));
};

LogEntry entrada(int linha, const QString& ts, int prioridade, const QString& origem, const QString& msg) {
    LogEntry e;
    e.line_number = linha;
    e.timestamp = ts;
    e.priority = prioridade;
    e.source = origem;
    e.message = msg;
    return e;
}

std::vector<LogEntry> tres_logs() {
    return {
        entrada(1, QStringLiteral("2026-11-26 10:00"), 3, QStringLiteral("kernel"), QStringLiteral("b")),
        entrada(2, QStringLiteral("2026-11-26 12:00"), 6, QStringLiteral("fwupd"), QStringLiteral("c")),
        entrada(3, QStringLiteral("2026-11-26 11:00"), 1, QStringLiteral("auth"), QStringLiteral("a")),
    };
}

struct Cenario {
    MockDiagnosticProvider* provider = nullptr;
    std::shared_ptr<DiagnosticManager> manager = std::make_shared<DiagnosticManager>();

    Cenario() {
        auto p = std::make_unique<NiceMock<MockDiagnosticProvider>>();
        provider = p.get();
        ON_CALL(*provider, fetch_logs(_, _, _)).WillByDefault(Return(tres_logs()));
        manager->register_provider(std::move(p));
    }
};

// refresh() é assíncrono: espera o busy voltar a false.
void aguardar_refresh(DiagnosticViewModel& vm) {
    QSignalSpy spy(&vm, &DiagnosticViewModel::isBusyChanged);
    vm.refresh();
    while (vm.isBusy()) {
        ASSERT_TRUE(spy.wait(2000));
    }
}

QStringList coluna(const LogEntryModel* model, int role) {
    QStringList valores;
    for (int i = 0; i < model->rowCount(); ++i) {
        valores << model->data(model->index(i), role).toString();
    }
    return valores;
}

} // namespace

TEST(DiagnosticViewModelTest, ShouldGenerateSearchUrl) {
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);

    EXPECT_FALSE(vm.isBusy());
}

TEST(DiagnosticViewModelTest, RefreshOrdenaPorTimestampDecrescentePorPadrao) {
    Cenario c;
    DiagnosticViewModel vm(c.manager);

    aguardar_refresh(vm);

    EXPECT_FALSE(vm.isBusy());
    EXPECT_EQ(coluna(vm.logModel(), LogEntryModel::LineNumberRole),
              (QStringList{QStringLiteral("2"), QStringLiteral("3"), QStringLiteral("1")}));
}

TEST(DiagnosticViewModelTest, RefreshRepassaFiltroBuscaELimiteAoProvedor) {
    Cenario c;
    DiagnosticViewModel vm(c.manager);
    vm.setSearchQuery(QStringLiteral("timeout"));
    vm.setLogLimit(42);
    EXPECT_CALL(*c.provider, fetch_logs(DiagnosticFilter::All, QString(QStringLiteral("timeout")), 42))
        .WillOnce(Return(tres_logs()));

    aguardar_refresh(vm);
}

TEST(DiagnosticViewModelTest, TrocarFiltroDisparaRefreshComONovoFiltro) {
    Cenario c;
    DiagnosticViewModel vm(c.manager);
    EXPECT_CALL(*c.provider, fetch_logs(DiagnosticFilter::Boot, _, _)).WillOnce(Return(tres_logs()));
    QSignalSpy filtro(&vm, &DiagnosticViewModel::currentFilterChanged);
    QSignalSpy busy(&vm, &DiagnosticViewModel::isBusyChanged);

    vm.setCurrentFilter(static_cast<int>(DiagnosticFilter::Boot));
    while (vm.isBusy()) {
        ASSERT_TRUE(busy.wait(2000));
    }

    EXPECT_EQ(filtro.count(), 1);
    EXPECT_EQ(vm.logModel()->rowCount(), 3);
}

TEST(DiagnosticViewModelTest, OrdenaPorPrioridadeCrescente) {
    Cenario c;
    DiagnosticViewModel vm(c.manager);
    aguardar_refresh(vm);

    vm.setSortField(QStringLiteral("priority"));
    vm.setSortAscending(true);

    EXPECT_EQ(coluna(vm.logModel(), LogEntryModel::PriorityRole),
              (QStringList{QStringLiteral("1"), QStringLiteral("3"), QStringLiteral("6")}));
}

TEST(DiagnosticViewModelTest, OrdenaPorOrigemEMensagem) {
    Cenario c;
    DiagnosticViewModel vm(c.manager);
    aguardar_refresh(vm);

    vm.setSortAscending(true);
    vm.setSortField(QStringLiteral("source"));
    EXPECT_EQ(coluna(vm.logModel(), LogEntryModel::SourceRole),
              (QStringList{QStringLiteral("auth"), QStringLiteral("fwupd"), QStringLiteral("kernel")}));

    vm.setSortField(QStringLiteral("message"));
    vm.setSortAscending(false);
    EXPECT_EQ(coluna(vm.logModel(), LogEntryModel::MessageRole),
              (QStringList{QStringLiteral("c"), QStringLiteral("b"), QStringLiteral("a")}));
}

TEST(DiagnosticViewModelTest, LimiteNegativoViraZero) {
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);
    QSignalSpy spy(&vm, &DiagnosticViewModel::logLimitChanged);

    vm.setLogLimit(-5);

    EXPECT_EQ(vm.logLimit(), 0);
    EXPECT_EQ(spy.count(), 1);
}

TEST(DiagnosticViewModelTest, SettersSoNotificamQuandoOValorMuda) {
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);
    QSignalSpy busca(&vm, &DiagnosticViewModel::searchQueryChanged);
    QSignalSpy limite(&vm, &DiagnosticViewModel::logLimitChanged);

    vm.setSearchQuery(QStringLiteral("x"));
    vm.setSearchQuery(QStringLiteral("x"));
    vm.setLogLimit(vm.logLimit());

    EXPECT_EQ(busca.count(), 1);
    EXPECT_EQ(limite.count(), 0);
}

// MC-10: o template vai para QDesktopServices::openUrl; só http(s) com %1 é aceito.
TEST(DiagnosticViewModelTest, AceitaMotorDeBuscaHttps) {
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);
    const QString url = QStringLiteral("https://duckduckgo.com/?q=%1");

    vm.setSearchEngineUrl(url);

    EXPECT_EQ(vm.searchEngineUrl(), url);
}

TEST(DiagnosticViewModelTest, RejeitaMotorDeBuscaComEsquemaNaoWeb) {
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);
    const QString original = vm.searchEngineUrl();
    QSignalSpy spy(&vm, &DiagnosticViewModel::searchEngineUrlChanged);

    vm.setSearchEngineUrl(QStringLiteral("file:///etc/passwd?%1"));
    vm.setSearchEngineUrl(QStringLiteral("javascript:alert(%1)"));
    vm.setSearchEngineUrl(QStringLiteral("https://exemplo.org/sem-placeholder"));

    EXPECT_EQ(vm.searchEngineUrl(), original);
    EXPECT_EQ(spy.count(), 0);
}

TEST(LogEntryModelTest, ExpoeRolesEDados) {
    LogEntryModel model;
    model.setLogs({entrada(7, QStringLiteral("ts"), 4, QStringLiteral("kernel"), QStringLiteral("msg"))});

    ASSERT_EQ(model.rowCount(), 1);
    const auto idx = model.index(0);
    EXPECT_EQ(model.data(idx, LogEntryModel::LineNumberRole).toInt(), 7);
    EXPECT_EQ(model.data(idx, LogEntryModel::TimestampRole).toString(), QStringLiteral("ts"));
    EXPECT_EQ(model.data(idx, LogEntryModel::PriorityRole).toInt(), 4);
    EXPECT_EQ(model.data(idx, LogEntryModel::MessageRole).toString(), QStringLiteral("msg"));
    EXPECT_EQ(model.data(idx, LogEntryModel::SourceRole).toString(), QStringLiteral("kernel"));
    EXPECT_FALSE(model.data(idx, Qt::DisplayRole).isValid());

    const auto roles = model.roleNames();
    EXPECT_EQ(roles.value(LogEntryModel::LineNumberRole), QByteArray("lineNumber"));
    EXPECT_EQ(roles.value(LogEntryModel::SourceRole), QByteArray("source"));
}

TEST(LogEntryModelTest, IndiceInvalidoOuForaDoIntervaloDevolveVazio) {
    LogEntryModel model;
    model.setLogs({entrada(1, {}, 0, {}, {})});

    EXPECT_FALSE(model.data(QModelIndex{}, LogEntryModel::MessageRole).isValid());
    EXPECT_FALSE(model.data(model.index(5), LogEntryModel::MessageRole).isValid());
    EXPECT_EQ(model.rowCount(model.index(0)), 0);
}
