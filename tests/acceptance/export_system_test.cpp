#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QSignalSpy>
#include "../../src/presentation/viewmodels/export_viewmodel.h"
#include "../../src/domain/interfaces/i_export_provider.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace testing;

class MockExportProvider : public IExportProvider {
public:
    MOCK_METHOD(bool, export_report, (const ReportData&, const QString&), (override));
    MOCK_METHOD(QString, get_format_name, (), (const, override));
};

TEST(ExportSystemAcceptance, ShouldConsolidateAndExportData) {
    int argc = 1;
    char* argv[] = {(char*)"test"};
    QCoreApplication app(argc, argv);

    ExportViewModel vm;
    auto mock = std::make_unique<MockExportProvider>();
    
    EXPECT_CALL(*mock, export_report(_, _)).WillOnce(Return(true));
    EXPECT_CALL(*mock, get_format_name()).WillRepeatedly(Return("Mock"));
    
    vm.add_provider(std::move(mock));
    
    QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
    
    vm.exportReport(0, "/tmp/test_report.json");
    
    // Como é assíncrono, aguardamos o sinal
    EXPECT_TRUE(spy.wait(2000));
    ASSERT_EQ(spy.count(), 1);
    
    bool success = spy.at(0).at(0).toBool();
    EXPECT_TRUE(success);
}
