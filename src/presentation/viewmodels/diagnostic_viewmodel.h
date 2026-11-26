#ifndef DIAGNOSTIC_VIEWMODEL_H
#define DIAGNOSTIC_VIEWMODEL_H

#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <memory>
#include "../../application/diagnostic_manager.h"
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
    Q_PROPERTY(int logLimit READ logLimit WRITE setLogLimit NOTIFY logLimitChanged)
    Q_PROPERTY(QString searchEngineUrl READ searchEngineUrl WRITE setSearchEngineUrl NOTIFY searchEngineUrlChanged)

public:
    explicit DiagnosticViewModel(std::shared_ptr<my_comp::application::DiagnosticManager> manager, QObject* parent = nullptr);

    [[nodiscard]] LogEntryModel* logModel() const { return m_model; }
    [[nodiscard]] bool isBusy() const { return m_is_busy; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void performSearch();
    Q_INVOKABLE void searchOnline(const QString& message);

    QString searchQuery() const { return m_search_query; }
    void setSearchQuery(const QString& q);

    int currentFilter() const { return m_current_filter; }
    void setCurrentFilter(int filter);

    QString sortField() const { return m_sort_field; }
    void setSortField(const QString& f);
    bool sortAscending() const { return m_sort_ascending; }
    void setSortAscending(bool a);

    int logLimit() const { return m_log_limit; }
    void setLogLimit(int n);

    QString searchEngineUrl() const { return m_search_engine_url; }
    void setSearchEngineUrl(const QString& url);

signals:
    void isBusyChanged();
    void searchQueryChanged();
    void currentFilterChanged();
    void sortFieldChanged();
    void sortAscendingChanged();
    void logLimitChanged();
    void searchEngineUrlChanged();

private:
    void applySortAndPush();
    std::shared_ptr<my_comp::application::DiagnosticManager> m_manager;
    LogEntryModel* m_model;
    std::vector<my_comp::domain::models::LogEntry> m_data;
    bool m_is_busy{false};
    QString m_search_query;
    int m_current_filter{5}; // Default: All
    QString m_sort_field{"timestamp"};
    bool m_sort_ascending{false};
    int m_log_limit{100};
    QString m_search_engine_url{"https://www.google.com/search?q=linux+%1"};
};

} // namespace my_comp::presentation::viewmodels

#endif // DIAGNOSTIC_VIEWMODEL_H
