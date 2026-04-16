#include "diagnostic_viewmodel.h"
#include <QtConcurrent>

namespace my_comp::presentation::viewmodels {

DiagnosticViewModel::DiagnosticViewModel(std::shared_ptr<my_comp::domain::DiagnosticManager> manager, QObject* parent)
    : QObject(parent), manager_(std::move(manager)), model_(new LogEntryModel(this)) {}

void DiagnosticViewModel::refresh() {
    if (is_busy_) return;
    is_busy_ = true;
    emit isBusyChanged();
    
    int filter = m_current_filter;
    QString query = search_query_;

    (void)QtConcurrent::run([this, filter, query]() {
        auto logs = manager_->fetch_all_logs(
            static_cast<my_comp::domain::DiagnosticFilter>(filter), 
            query
        );
        
        QMetaObject::invokeMethod(this, [this, logs]() mutable {
            model_->setLogs(std::move(logs));
            is_busy_ = false;
            emit isBusyChanged();
        }, Qt::QueuedConnection);
    });
}

void DiagnosticViewModel::performSearch() {
    refresh();
}

void DiagnosticViewModel::searchOnline(const QString& message) {
    if (message.isEmpty()) return;
    
    // Remover cabeçalhos comuns (timestamp e hostname) se presentes para busca mais limpa
    // Mas o JournalDiagnosticProvider já entrega a mensagem limpa.
    
    QUrl url("https://www.google.com/search");
    QUrlQuery query;
    // Adicionamos "linux" e "journalctl" para dar contexto à busca
    query.addQueryItem("q", "linux " + message);
    url.setQuery(query);
    
    qDebug() << "[DiagnosticVM] Opening search for:" << message;
    QDesktopServices::openUrl(url);
}

void DiagnosticViewModel::setSearchQuery(const QString& q) {
    if (search_query_ != q) {
        search_query_ = q;
        emit searchQueryChanged();
    }
}

void DiagnosticViewModel::setCurrentFilter(int filter) {
    if (m_current_filter != filter) {
        m_current_filter = filter; 
        emit currentFilterChanged();
        refresh();
    }
}

} // namespace my_comp::presentation::viewmodels
