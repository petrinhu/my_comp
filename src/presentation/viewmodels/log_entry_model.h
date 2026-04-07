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
        TimestampRole = Qt::UserRole + 1,
        PriorityRole,
        MessageRole,
        SourceRole
    };

    explicit LogEntryModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    void setLogs(std::vector<LogEntry> logs) {
        beginResetModel();
        logs_ = std::move(logs);
        endResetModel();
    }

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return static_cast<int>(logs_.size());
    }

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || index.row() >= static_cast<int>(logs_.size()))
            return {};

        const auto& log = logs_[static_cast<size_t>(index.row())];
        switch (role) {
            case TimestampRole: return log.timestamp;
            case PriorityRole: return log.priority;
            case MessageRole: return log.message;
            case SourceRole: return log.source;
            default: return {};
        }
    }

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TimestampRole] = "timestamp";
        roles[PriorityRole] = "priority";
        roles[MessageRole] = "message";
        roles[SourceRole] = "source";
        return roles;
    }

private:
    std::vector<LogEntry> logs_;
};

} // namespace my_comp::presentation::viewmodels

#endif // LOG_ENTRY_MODEL_H
