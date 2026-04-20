#ifndef DIAGNOSTIC_VIEWMODEL_H
#define DIAGNOSTIC_VIEWMODEL_H

#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <memory>
#include "../../domain/diagnostic_manager.h"
#include "log_entry_model.h"

namespace my_comp::presentation::viewmodels {

class DiagnosticViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(LogEntryModel* logModel READ logModel CONSTANT)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(int currentFilter READ currentFilter WRITE setCurrentFilter NOTIFY currentFilterChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)

public:
    explicit DiagnosticViewModel(std::shared_ptr<my_comp::domain::DiagnosticManager> manager, QObject* parent = nullptr);

    [[nodiscard]] LogEntryModel* logModel() const { return model_; }
    [[nodiscard]] bool isBusy() const { return is_busy_; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void performSearch();
    Q_INVOKABLE void searchOnline(const QString& message);

    QString searchQuery() const { return search_query_; }
    void setSearchQuery(const QString& q);

    int currentFilter() const { return m_current_filter; }
    void setCurrentFilter(int filter);

    QString sortField() const { return sort_field_; }
    void setSortField(const QString& f);
    bool sortAscending() const { return sort_ascending_; }
    void setSortAscending(bool a);

signals:
    void isBusyChanged();
    void searchQueryChanged();
    void currentFilterChanged();
    void sortFieldChanged();
    void sortAscendingChanged();

private:
    void applySortAndPush();
    std::shared_ptr<my_comp::domain::DiagnosticManager> manager_;
    LogEntryModel* model_;
    std::vector<my_comp::domain::models::LogEntry> data_;
    bool is_busy_{false};
    QString search_query_;
    int m_current_filter{5}; // Default: All
    QString sort_field_{"timestamp"};
    bool sort_ascending_{false};
};

} // namespace my_comp::presentation::viewmodels

#endif // DIAGNOSTIC_VIEWMODEL_H
