#ifndef JOURNAL_DIAGNOSTIC_PROVIDER_H
#define JOURNAL_DIAGNOSTIC_PROVIDER_H

#include <memory>
#include "../../domain/interfaces/i_diagnostic_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

using IDiagnosticProvider = my_comp::domain::interfaces::IDiagnosticProvider;
using LogEntry = my_comp::domain::models::LogEntry;
using DiagnosticFilter = my_comp::domain::interfaces::DiagnosticFilter;

class JournalDiagnosticProvider : public IDiagnosticProvider {
public:
    explicit JournalDiagnosticProvider(std::shared_ptr<IShellExecutor> shell) : shell_(std::move(shell)) {}

    [[nodiscard]] QString get_name() const override { return "journal"; }

    [[nodiscard]] std::vector<LogEntry> fetch_logs(DiagnosticFilter filter = DiagnosticFilter::All, const QString& query = "") override {
        QStringList args = {"-b", "--no-pager", "-n", "100"}; // Limite de 100 para performance
        
        switch (filter) {
            case DiagnosticFilter::CriticalErrors:
                args << "-p" << "3";
                break;
            case DiagnosticFilter::HardwareFirmware:
                args << "_TRANSPORT=kernel" << "SYSLOG_IDENTIFIER=fwupd" << "SYSLOG_IDENTIFIER=smartd" << "SYSLOG_IDENTIFIER=mdadm";
                break;
            case DiagnosticFilter::Boot:
                args << "_SYSTEMD_UNIT=systemd-modules-load.service" << "_SYSTEMD_UNIT=systemd-sysctl.service";
                break;
            case DiagnosticFilter::DesktopEnvironment:
                args << "_COMM=kwin_wayland" << "_COMM=plasmashell";
                break;
            case DiagnosticFilter::SecurityNetwork:
                args << "_SYSTEMD_UNIT=firewalld.service" << "_SYSTEMD_UNIT=sshd.service" << "SYSLOG_IDENTIFIER=sudo";
                break;
            case DiagnosticFilter::All:
            default:
                break;
        }

        if (!query.isEmpty()) {
            args << "-g" << query;
        }

        QString output;
        shell_->execute("journalctl", args, output);
        
        std::vector<LogEntry> logs;
        auto lines = output.split("\n", Qt::SkipEmptyParts);
        for (const auto& line : lines) {
            logs.push_back({
                "Live",           // timestamp
                3,                // level
                line.trimmed(),   // message
                "journal"         // source
            });
        }
        return logs;
    }

private:
    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // JOURNAL_DIAGNOSTIC_PROVIDER_H
