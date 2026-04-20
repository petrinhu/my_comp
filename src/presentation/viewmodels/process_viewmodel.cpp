#include "process_viewmodel.h"
#include <algorithm>
#include <QPointer>
#include <QVariantMap>

namespace my_comp::presentation::viewmodels {

ProcessViewModel::ProcessViewModel(std::shared_ptr<domain::interfaces::IProcessProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &ProcessViewModel::refresh);
}

QVariantList ProcessViewModel::processes() const {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return m_processedList;
}

void ProcessViewModel::setFilter(const QString& f) {
    if (m_filter != f) {
        m_filter = f;
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);
            rebuildProcessedListLocked();
        }
        emit filterChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::setSortField(const QString& field) {
    if (m_sortField != field) {
        m_sortField = field;
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);
            std::stable_sort(m_data.begin(), m_data.end(), [this](const auto& a, const auto& b) {
                if (m_sortField == "cpu")  return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
                if (m_sortField == "mem")  return m_sortAscending ? (a.memPercent < b.memPercent) : (a.memPercent > b.memPercent);
                if (m_sortField == "pid")  return m_sortAscending ? (a.pid < b.pid) : (a.pid > b.pid);
                if (m_sortField == "user") return m_sortAscending ? (a.user < b.user) : (a.user > b.user);
                return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
            });
            rebuildProcessedListLocked();
        }
        emit sortFieldChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::setSortAscending(bool asc) {
    if (m_sortAscending != asc) {
        m_sortAscending = asc;
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);
            std::stable_sort(m_data.begin(), m_data.end(), [this](const auto& a, const auto& b) {
                if (m_sortField == "cpu")  return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
                if (m_sortField == "mem")  return m_sortAscending ? (a.memPercent < b.memPercent) : (a.memPercent > b.memPercent);
                if (m_sortField == "pid")  return m_sortAscending ? (a.pid < b.pid) : (a.pid > b.pid);
                if (m_sortField == "user") return m_sortAscending ? (a.user < b.user) : (a.user > b.user);
                return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
            });
            rebuildProcessedListLocked();
        }
        emit sortAscendingChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::rebuildProcessedListLocked() {
    QVariantList newList;
    newList.reserve(static_cast<int>(m_data.size()));
    for (const auto& p : m_data) {
        if (!m_filter.isEmpty() && !p.command.contains(m_filter, Qt::CaseInsensitive))
            continue;
        QVariantMap map;
        map["pid"] = p.pid;
        map["user"] = p.user;
        map["priority"] = p.priority;
        map["virt"] = static_cast<double>(p.virt);
        map["res"] = static_cast<double>(p.res);
        map["state"] = (p.state == domain::models::ProcessState::Running) ? "R" : "S";
        map["cpu"] = p.cpuPercent;
        map["mem"] = p.memPercent;
        map["time"] = p.timeStr;
        map["command"] = p.command;
        newList.append(map);
    }
    m_processedList = std::move(newList);
}

void ProcessViewModel::refresh() {
    if (m_isBusy.exchange(true)) return;

    if (m_refreshTimer && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(2000);
    }

    QString sortField = m_sortField;
    bool sortAscending = m_sortAscending;
    QString filter = m_filter;

    QPointer<ProcessViewModel> self(this);
    auto provider = m_provider;
    (void)QtConcurrent::run([self, provider, sortField, sortAscending]() {
        auto raw_data = provider->collect_processes();

        // Ordenação estável na thread separada (mantém ordem relativa em empates)
        std::stable_sort(raw_data.begin(), raw_data.end(), [sortField, sortAscending](const auto& a, const auto& b) {
            if (sortField == "cpu")  return sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
            if (sortField == "mem")  return sortAscending ? (a.memPercent < b.memPercent) : (a.memPercent > b.memPercent);
            if (sortField == "pid")  return sortAscending ? (a.pid < b.pid) : (a.pid > b.pid);
            if (sortField == "user") return sortAscending ? (a.user < b.user) : (a.user > b.user);
            return sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
        });

        QMetaObject::invokeMethod(self.data(), [self, raw_data]() mutable {
            if (!self) return;
            {
                std::lock_guard<std::mutex> lock(self->m_dataMutex);
                self->m_data = std::move(raw_data);
                self->rebuildProcessedListLocked();
            }
            self->m_isBusy = false;
            emit self->processesChanged();
        }, Qt::QueuedConnection);
    });
}

void ProcessViewModel::killProcess(int pid, int signal) {
    if (!m_provider) return;
    if (m_provider->kill_process(pid, signal)) {
        refresh();
    }
}

void ProcessViewModel::setPriority(int pid, int nice) {
    if (!m_provider) return;
    if (m_provider->set_priority(pid, nice)) {
        refresh();
    }
}

void ProcessViewModel::sortData() {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    std::stable_sort(m_data.begin(), m_data.end(), [this](const auto& a, const auto& b) {
        if (m_sortField == "cpu") {
            return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
        } else if (m_sortField == "mem") {
            return m_sortAscending ? (a.memPercent < b.memPercent) : (a.memPercent > b.memPercent);
        } else if (m_sortField == "pid") {
            return m_sortAscending ? (a.pid < b.pid) : (a.pid > b.pid);
        } else if (m_sortField == "user") {
            return m_sortAscending ? (a.user < b.user) : (a.user > b.user);
        }
        return m_sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
    });
}

} // namespace my_comp::presentation::viewmodels
