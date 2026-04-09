#ifndef DIAGNOSTIC_VIEWMODEL_H
#define DIAGNOSTIC_VIEWMODEL_H

#include <QObject>
#include <QDebug>
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

public:
    explicit DiagnosticViewModel(std::shared_ptr<my_comp::domain::DiagnosticManager> manager, QObject* parent = nullptr)
        : QObject(parent), manager_(std::move(manager)), model_(new LogEntryModel(this)) {}

    [[nodiscard]] LogEntryModel* logModel() const { return model_; }
    [[nodiscard]] bool isBusy() const { return is_busy_; }

    Q_INVOKABLE void refresh() {
        if (is_busy_) return;
        is_busy_ = true;
        emit isBusyChanged();
        
        auto logs = manager_->fetch_all_logs(
            static_cast<my_comp::domain::DiagnosticFilter>(m_current_filter), 
            search_query_
        );
        
        model_->setLogs(std::move(logs));
        is_busy_ = false;
        emit isBusyChanged();
    }

    Q_INVOKABLE void performSearch() {
        refresh();
    }

    Q_INVOKABLE void searchOnline(const QString& message) {
        if (message.isEmpty()) return;
        
        QUrl url("https://www.google.com/search");
        QUrlQuery query;
        query.addQueryItem("q", "linux journalctl " + message);
        url.setQuery(query);
        
        QDesktopServices::openUrl(url);
    }

    QString searchQuery() const { return search_query_; }
    void setSearchQuery(const QString& q) { search_query_ = q; emit searchQueryChanged(); }
    
    int currentFilter() const { return m_current_filter; }
    void setCurrentFilter(int filter) { 
        if (m_current_filter != filter) {
            m_current_filter = filter; 
            emit currentFilterChanged();
            refresh();
        }
    }

signals:
    void isBusyChanged();
    void searchQueryChanged();
    void currentFilterChanged();

private:
    std::shared_ptr<my_comp::domain::DiagnosticManager> manager_;
    LogEntryModel* model_;
    bool is_busy_{false};
    QString search_query_;
    int m_current_filter{5}; // Default: All
};

} // namespace my_comp::presentation::viewmodels

#endif // DIAGNOSTIC_VIEWMODEL_H
