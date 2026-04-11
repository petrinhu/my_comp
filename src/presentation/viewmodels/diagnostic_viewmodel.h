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
