#include "diagnostic_viewmodel.h"
#include <QDebug>
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

    // setSearchEngineUrl() garante http(s) com o placeholder %1.
    const QString encoded = QUrl::toPercentEncoding(message);
    const QString final_url = QString(m_search_engine_url).replace(QStringLiteral("%1"), encoded);

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

namespace {
// MC-10: o template vai para QDesktopServices::openUrl. Só http(s) com o
// placeholder %1 é aceito; file://, javascript: e afins abririam outro app.
bool is_safe_search_template(const QString& url) {
    if (!url.contains(QStringLiteral("%1"))) {
        return false;
    }
    const QUrl parsed{QString(url).replace(QStringLiteral("%1"), QStringLiteral("x"))};
    const QString scheme = parsed.scheme().toLower();
    return parsed.isValid() && !parsed.host().isEmpty()
        && (scheme == QStringLiteral("https") || scheme == QStringLiteral("http"));
}
} // namespace

void DiagnosticViewModel::setSearchEngineUrl(const QString& url) {
    if (!is_safe_search_template(url)) {
        qWarning() << "[DiagnosticVM] Template de busca rejeitado (precisa ser http(s) com %1)";
        return;
    }
    if (m_search_engine_url != url) {
        m_search_engine_url = url;
        emit searchEngineUrlChanged();
    }
}

} // namespace my_comp::presentation::viewmodels
