#pragma once
#include "../../domain/interfaces/i_export_provider.h"

namespace my_comp::infrastructure::exporters {

class JsonExportProvider : public domain::interfaces::IExportProvider {
public:
    bool export_report(const domain::models::ReportData& data, const QString& path) override;
    QString get_format_name() const override { return "JSON"; }
};

} // namespace my_comp::infrastructure::exporters
