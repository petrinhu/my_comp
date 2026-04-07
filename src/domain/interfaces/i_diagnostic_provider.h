#ifndef I_DIAGNOSTIC_PROVIDER_H
#define I_DIAGNOSTIC_PROVIDER_H

#include <vector>
#include <QString>
#include "../models/log_entry.h"

namespace my_comp::domain::interfaces {

using LogEntry = my_comp::domain::models::LogEntry;

enum class DiagnosticFilter {
    CriticalErrors,
    HardwareFirmware,
    Boot,
    DesktopEnvironment,
    SecurityNetwork,
    All
};

class IDiagnosticProvider {
public:
    virtual ~IDiagnosticProvider() = default;

    [[nodiscard]] virtual QString get_name() const = 0;
    [[nodiscard]] virtual std::vector<LogEntry> fetch_logs(DiagnosticFilter filter = DiagnosticFilter::All) = 0;
    [[nodiscard]] virtual std::vector<LogEntry> search_logs(const QString& query) = 0;
};

} // namespace my_comp::domain::interfaces

#endif // I_DIAGNOSTIC_PROVIDER_H
