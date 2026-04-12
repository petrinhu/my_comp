#include <gtest/gtest.h>
#include <QFile>
#include <QTextStream>
#include "../../src/infrastructure/exporters/json_export_provider.h"
#include "../../src/infrastructure/exporters/markdown_export_provider.h"
#include "../../src/infrastructure/exporters/html_export_provider.h"

using namespace my_comp::infrastructure::exporters;
using namespace my_comp::domain::models;

class ExportersTest : public ::testing::Test {
protected:
    ReportData sample_data;
    void SetUp() override {
        sample_data.title = "Unit Test Report";
        sample_data.timestamp = "2026-04-11 12:00:00";
        sample_data.distro = "Fedora 43";
        sample_data.kernel = "6.12.0";
        sample_data.user = "petrus";
        
        QVariantMap section1;
        section1["CPU Load"] = "15%";
        section1["RAM Free"] = "8GB";
        sample_data.sections["Overview"] = section1;
    }
};

TEST_F(ExportersTest, JsonExportShouldCreateFile) {
    JsonExportProvider exporter;
    QString path = "test_report.json";
    EXPECT_TRUE(exporter.export_report(sample_data, path));
    EXPECT_TRUE(QFile::exists(path));
    QFile::remove(path);
}

TEST_F(ExportersTest, MarkdownExportShouldCreateFile) {
    MarkdownExportProvider exporter;
    QString path = "test_report.md";
    EXPECT_TRUE(exporter.export_report(sample_data, path));
    EXPECT_TRUE(QFile::exists(path));
    
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = QTextStream(&file).readAll();
        EXPECT_TRUE(content.contains("# Unit Test Report"));
        EXPECT_TRUE(content.contains("petrus"));
    }
    QFile::remove(path);
}

TEST_F(ExportersTest, HtmlExportShouldCreateFile) {
    HtmlExportProvider exporter;
    QString path = "test_report.html";
    EXPECT_TRUE(exporter.export_report(sample_data, path));
    EXPECT_TRUE(QFile::exists(path));
    
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = QTextStream(&file).readAll();
        EXPECT_TRUE(content.contains("<html>"));
        EXPECT_TRUE(content.contains("Unit Test Report"));
    }
    QFile::remove(path);
}
