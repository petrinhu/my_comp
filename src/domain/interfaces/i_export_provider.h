#pragma once
#include "../models/report_data.h"

namespace my_comp::domain::interfaces {

class IExportProvider {
public:
    virtual ~IExportProvider() = default;
    virtual bool export_report(const models::ReportData& data, const QString& path) = 0;
    virtual QString get_format_name() const = 0;
};

} // namespace my_comp::domain::interfaces
