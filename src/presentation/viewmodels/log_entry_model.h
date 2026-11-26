#ifndef LOG_ENTRY_MODEL_H
#define LOG_ENTRY_MODEL_H

#include <QAbstractListModel>
#include <vector>
#include "../../domain/models/log_entry.h"

namespace my_comp::presentation::viewmodels {

using LogEntry = my_comp::domain::models::LogEntry;

class LogEntryModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum LogRoles {
        LineNumberRole = Qt::UserRole + 1,
        TimestampRole,
        PriorityRole,
        MessageRole,
        SourceRole
    };

    explicit LogEntryModel(QObject* parent = nullptr);

    void setLogs(std::vector<LogEntry> logs);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<LogEntry> m_logs;
};

} // namespace my_comp::presentation::viewmodels

#endif // LOG_ENTRY_MODEL_H
