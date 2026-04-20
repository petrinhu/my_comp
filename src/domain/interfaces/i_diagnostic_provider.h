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

    /**
     * @brief Coleta logs combinando filtro de categoria e busca por texto.
     * @param filter Categoria (ex: Hardware, Boot).
     * @param query Texto opcional para busca (ex: "timeout", "error").
     * @param limit Número máximo de linhas a retornar (0 = ilimitado).
     */
    virtual std::vector<LogEntry> fetch_logs(DiagnosticFilter filter = DiagnosticFilter::All, const QString& query = "", int limit = 100) = 0;
};

} // namespace my_comp::domain::interfaces

#endif // I_DIAGNOSTIC_PROVIDER_H
