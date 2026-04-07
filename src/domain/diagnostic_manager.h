#ifndef DIAGNOSTIC_MANAGER_H
#define DIAGNOSTIC_MANAGER_H

#include <vector>
#include <memory>
#include <QString>
#include "interfaces/i_diagnostic_provider.h"
#include "models/log_entry.h"

namespace my_comp::domain {

using IDiagnosticProvider = interfaces::IDiagnosticProvider;
using LogEntry = models::LogEntry;
using DiagnosticFilter = interfaces::DiagnosticFilter;

class DiagnosticManager {
public:
    void register_provider(std::unique_ptr<IDiagnosticProvider> provider) {
        providers_.push_back(std::move(provider));
    }

    [[nodiscard]] std::vector<LogEntry> fetch_all_logs(DiagnosticFilter filter = DiagnosticFilter::All) {
        std::vector<LogEntry> all_logs;
        for (const auto& provider : providers_) {
            auto logs = provider->fetch_logs(filter);
            all_logs.insert(all_logs.end(), 
                          std::make_move_iterator(logs.begin()), 
                          std::make_move_iterator(logs.end()));
        }
        return all_logs;
    }

    [[nodiscard]] std::vector<LogEntry> search_logs(const QString& query) {
        std::vector<LogEntry> all_logs;
        for (const auto& provider : providers_) {
            auto logs = provider->search_logs(query);
            all_logs.insert(all_logs.end(), 
                          std::make_move_iterator(logs.begin()), 
                          std::make_move_iterator(logs.end()));
        }
        return all_logs;
    }

private:
    std::vector<std::unique_ptr<IDiagnosticProvider>> providers_;
};

} // namespace my_comp::domain

#endif // DIAGNOSTIC_MANAGER_H
