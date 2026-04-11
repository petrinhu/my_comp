#include "log_entry_model.h"

namespace my_comp::presentation::viewmodels {

LogEntryModel::LogEntryModel(QObject* parent) : QAbstractListModel(parent) {}

void LogEntryModel::setLogs(std::vector<LogEntry> logs) {
    beginResetModel();
    logs_ = std::move(logs);
    endResetModel();
}

int LogEntryModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(logs_.size());
}

QVariant LogEntryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(logs_.size()))
        return {};

    const auto& log = logs_[static_cast<size_t>(index.row())];
    switch (role) {
        case LineNumberRole: return log.line_number;
        case TimestampRole: return log.timestamp;
        case PriorityRole: return log.priority;
        case MessageRole: return log.message;
        case SourceRole: return log.source;
        default: return {};
    }
}

QHash<int, QByteArray> LogEntryModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[LineNumberRole] = "lineNumber";
    roles[TimestampRole] = "timestamp";
    roles[PriorityRole] = "priority";
    roles[MessageRole] = "message";
    roles[SourceRole] = "source";
    return roles;
}

} // namespace my_comp::presentation::viewmodels
