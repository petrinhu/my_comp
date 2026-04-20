#ifndef JOURNAL_DIAGNOSTIC_PROVIDER_H
#define JOURNAL_DIAGNOSTIC_PROVIDER_H

#include <memory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>
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

    [[nodiscard]] std::vector<LogEntry> fetch_logs(DiagnosticFilter filter = DiagnosticFilter::All, const QString& query = "", int limit = 100) override {
        QStringList args = {"-b", "--no-pager", "-o", "json"};
        if (limit > 0) {
            args << "-n" << QString::number(limit);
        }

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
        int current_line = 1;
        for (const auto& line : lines) {
            const QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) continue;

            LogEntry entry{current_line++, "Live", 3, trimmed, "journal"};

            if (trimmed.startsWith('{')) {
                QJsonParseError err;
                auto doc = QJsonDocument::fromJson(trimmed.toUtf8(), &err);
                if (err.error == QJsonParseError::NoError && doc.isObject()) {
                    const auto obj = doc.object();

                    // MESSAGE pode ser string, array de bytes, ou ausente
                    if (obj.contains("MESSAGE")) {
                        const auto msg = obj.value("MESSAGE");
                        if (msg.isString()) {
                            entry.message = msg.toString();
                        } else if (msg.isArray()) {
                            QByteArray raw;
                            for (const auto& v : msg.toArray()) raw.append(static_cast<char>(v.toInt()));
                            entry.message = QString::fromUtf8(raw);
                        }
                    }

                    // __REALTIME_TIMESTAMP em microssegundos desde epoch (string)
                    if (obj.contains("__REALTIME_TIMESTAMP")) {
                        bool ok = false;
                        const qint64 us = obj.value("__REALTIME_TIMESTAMP").toString().toLongLong(&ok);
                        if (ok && us > 0) {
                            const auto dt = QDateTime::fromMSecsSinceEpoch(us / 1000);
                            entry.timestamp = dt.toString("MMM dd HH:mm:ss");
                        }
                    }

                    // PRIORITY vem como string "0".."7"
                    if (obj.contains("PRIORITY")) {
                        bool ok = false;
                        const int p = obj.value("PRIORITY").toString().toInt(&ok);
                        if (ok) entry.priority = p;
                    }

                    // Fonte preferida: SYSLOG_IDENTIFIER, fallback _COMM
                    if (obj.contains("SYSLOG_IDENTIFIER")) {
                        entry.source = obj.value("SYSLOG_IDENTIFIER").toString();
                    } else if (obj.contains("_COMM")) {
                        entry.source = obj.value("_COMM").toString();
                    }
                }
            }

            logs.push_back(std::move(entry));
        }
        return logs;
    }

private:
    std::shared_ptr<IShellExecutor> shell_;
};

} // namespace my_comp::infrastructure::collectors

#endif // JOURNAL_DIAGNOSTIC_PROVIDER_H
