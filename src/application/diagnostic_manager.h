#ifndef DIAGNOSTIC_MANAGER_H
#define DIAGNOSTIC_MANAGER_H

#include <vector>
#include <memory>
#include <QString>
#include "../domain/interfaces/i_diagnostic_provider.h"
#include "../domain/models/log_entry.h"

namespace my_comp::application {

using IDiagnosticProvider = domain::interfaces::IDiagnosticProvider;
using LogEntry = domain::models::LogEntry;
using DiagnosticFilter = domain::interfaces::DiagnosticFilter;

class DiagnosticManager {
public:
    void register_provider(std::unique_ptr<IDiagnosticProvider> provider) {
        m_providers.push_back(std::move(provider));
    }

    /**
     * @brief Coleta logs de todos os provedores com filtro, busca e limite aplicados.
     */
    [[nodiscard]] std::vector<LogEntry> fetch_all_logs(DiagnosticFilter filter = DiagnosticFilter::All, const QString& query = "", int limit = 100) {
        std::vector<LogEntry> all_logs;
        for (const auto& provider : m_providers) {
            auto logs = provider->fetch_logs(filter, query, limit);
            all_logs.insert(all_logs.end(),
                          std::make_move_iterator(logs.begin()),
                          std::make_move_iterator(logs.end()));
        }
        return all_logs;
    }

private:
    std::vector<std::unique_ptr<IDiagnosticProvider>> m_providers;
};

} // namespace my_comp::application

#endif // DIAGNOSTIC_MANAGER_H
