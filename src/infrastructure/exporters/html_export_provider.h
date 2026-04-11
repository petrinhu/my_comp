#pragma once
#include "../../domain/interfaces/i_export_provider.h"

namespace my_comp::infrastructure::exporters {

class HtmlExportProvider : public domain::interfaces::IExportProvider {
public:
    bool export_report(const domain::models::ReportData& data, const QString& path) override;
    QString get_format_name() const override { return "HTML"; }

private:
    QString to_html_table(const QVariant& v);
};

} // namespace my_comp::infrastructure::exporters
