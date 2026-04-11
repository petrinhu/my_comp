#pragma once
#include "../../domain/interfaces/i_export_provider.h"

namespace my_comp::infrastructure::exporters {

class MarkdownExportProvider : public domain::interfaces::IExportProvider {
public:
    bool export_report(const domain::models::ReportData& data, const QString& path) override;
    QString get_format_name() const override { return "Markdown"; }

private:
    QString format_variant(const QVariant& v, int level = 0);
};

} // namespace my_comp::infrastructure::exporters
