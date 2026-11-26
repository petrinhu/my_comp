#include "diagnostic_viewmodel.h"
#include <QPointer>
#include <QtConcurrent>
#include <algorithm>

namespace my_comp::presentation::viewmodels {

DiagnosticViewModel::DiagnosticViewModel(std::shared_ptr<my_comp::application::DiagnosticManager> manager, QObject* parent)
    : QObject(parent), m_manager(std::move(manager)), m_model(new LogEntryModel(this)) {}

void DiagnosticViewModel::refresh() {
    if (m_is_busy) return;
    m_is_busy = true;
    emit isBusyChanged();

    const int filter = m_current_filter;
    const QString query = m_search_query;
    const int limit = m_log_limit;

    const QPointer<DiagnosticViewModel> self(this);
    auto manager = m_manager;
    (void)QtConcurrent::run([self, manager, filter, query, limit]() {
        auto logs = manager->fetch_all_logs(
            static_cast<my_comp::application::DiagnosticFilter>(filter),
            query,
            limit
        );

        QMetaObject::invokeMethod(self.data(), [self, logs]() mutable {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_data = std::move(logs);
            vm->applySortAndPush();
            vm->m_is_busy = false;
            emit vm->isBusyChanged();
        }, Qt::QueuedConnection);
    });
}

void DiagnosticViewModel::setSortField(const QString& f) {
    if (m_sort_field != f) {
        m_sort_field = f;
        emit sortFieldChanged();
        applySortAndPush();
    }
}

void DiagnosticViewModel::setSortAscending(bool a) {
    if (m_sort_ascending != a) {
        m_sort_ascending = a;
        emit sortAscendingChanged();
        applySortAndPush();
    }
}

void DiagnosticViewModel::applySortAndPush() {
    auto sorted = m_data;
    const QString field = m_sort_field;
    const bool asc = m_sort_ascending;
    std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
        if (field == "priority") return asc ? (a.priority < b.priority) : (a.priority > b.priority);
        if (field == "source")   return asc ? (a.source < b.source)     : (a.source > b.source);
        if (field == "message")  return asc ? (a.message < b.message)   : (a.message > b.message);
        return asc ? (a.timestamp < b.timestamp) : (a.timestamp > b.timestamp);
    });
    m_model->setLogs(std::move(sorted));
}

void DiagnosticViewModel::performSearch() {
    refresh();
}

void DiagnosticViewModel::searchOnline(const QString& message) {
    if (message.isEmpty()) return;

    // Template deve conter %1 como placeholder do termo de busca.
    // Fallback seguro: se não houver %1 e a URL for válida, apenda como query.
    const QString encoded = QUrl::toPercentEncoding(message);
    QString final_url;
    if (m_search_engine_url.contains("%1")) {
        final_url = QString(m_search_engine_url).replace("%1", encoded);
    } else {
        final_url = m_search_engine_url + encoded;
    }

    qDebug() << "[DiagnosticVM] Opening search for:" << message << "via" << final_url;
    QDesktopServices::openUrl(QUrl(final_url));
}

void DiagnosticViewModel::setSearchQuery(const QString& q) {
    if (m_search_query != q) {
        m_search_query = q;
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

void DiagnosticViewModel::setLogLimit(int n) {
    if (n < 0) n = 0;
    if (m_log_limit != n) {
        m_log_limit = n;
        emit logLimitChanged();
    }
}

void DiagnosticViewModel::setSearchEngineUrl(const QString& url) {
    if (m_search_engine_url != url) {
        m_search_engine_url = url;
        emit searchEngineUrlChanged();
    }
}

} // namespace my_comp::presentation::viewmodels
