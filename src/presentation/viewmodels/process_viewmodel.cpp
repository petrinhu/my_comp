#include "process_viewmodel.h"
#include <algorithm>
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
        emit filterChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::setSortField(const QString& field) {
    if (m_sortField != field) {
        m_sortField = field;
        sortData();
        emit sortFieldChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::setSortAscending(bool asc) {
    if (m_sortAscending != asc) {
        m_sortAscending = asc;
        sortData();
        emit sortAscendingChanged();
        emit processesChanged();
    }
}

void ProcessViewModel::refresh() {
    if (m_isBusy.exchange(true)) return;

    if (m_refreshTimer && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(2000);
    }

    QString sortField = m_sortField;
    bool sortAscending = m_sortAscending;
    QString filter = m_filter;

    (void)QtConcurrent::run([this, sortField, sortAscending, filter]() {
        auto raw_data = m_provider->collect_processes();
        
        // Ordenação na thread separada
        std::sort(raw_data.begin(), raw_data.end(), [sortField, sortAscending](const auto& a, const auto& b) {
            if (sortField == "cpu") {
                return sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
            } else if (sortField == "mem") {
                return sortAscending ? (a.memPercent < b.memPercent) : (a.memPercent > b.memPercent);
            } else if (sortField == "pid") {
                return sortAscending ? (a.pid < b.pid) : (a.pid > b.pid);
            } else if (sortField == "user") {
                return sortAscending ? (a.user < b.user) : (a.user > b.user);
            }
            return sortAscending ? (a.cpuPercent < b.cpuPercent) : (a.cpuPercent > b.cpuPercent);
        });

        // Gerar a QVariantList (Snapshot) na thread de background
        QVariantList newList;
        for (const auto& p : raw_data) {
            if (!filter.isEmpty() && !p.command.contains(filter, Qt::CaseInsensitive))
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

        QMetaObject::invokeMethod(this, [this, raw_data, newList]() {
            {
                std::lock_guard<std::mutex> lock(m_dataMutex);
                m_data = raw_data;
                m_processedList = newList; // Novo membro para o QML consumir
            }
            m_isBusy = false;
            emit processesChanged();
        }, Qt::QueuedConnection);
    });
}

void ProcessViewModel::killProcess(int pid, int signal) {
    if (m_provider->kill_process(pid, signal)) {
        refresh();
    }
}

void ProcessViewModel::setPriority(int pid, int nice) {
    if (m_provider->set_priority(pid, nice)) {
        refresh();
    }
}

void ProcessViewModel::sortData() {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    std::sort(m_data.begin(), m_data.end(), [this](const auto& a, const auto& b) {
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
