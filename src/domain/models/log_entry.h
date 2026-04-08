#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <QString>

namespace my_comp::domain::models {

struct LogEntry {
    int line_number;
    QString timestamp;
    int priority;        // 0-7 (systemd priority)
    QString message;
    QString source;      // e.g., "kernel", "fwupd"
};

} // namespace my_comp::domain::models

#endif // LOG_ENTRY_H
