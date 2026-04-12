#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QtTest/QSignalSpy>
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

TEST(ExportViewModelTest, ShouldHandleExport) {
    ExportViewModel vm;
    auto provider = std::make_unique<MockExportProvider>();
    
    EXPECT_CALL(*provider, get_format_name()).WillRepeatedly(Return("Mock"));
    EXPECT_CALL(*provider, export_report(_, _)).WillOnce(Return(true));
    
    vm.add_provider(std::move(provider));
    
    QSignalSpy spy(&vm, &ExportViewModel::exportFinished);
    vm.exportReport(0, "test.mock");
    spy.wait(2000);
    
    EXPECT_FALSE(spy.isEmpty());
}
