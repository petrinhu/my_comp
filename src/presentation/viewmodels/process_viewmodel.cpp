#include "process_viewmodel.h"
#include <algorithm>
#include <QVariantMap>

namespace my_comp::presentation::viewmodels {

ProcessViewModel::ProcessViewModel(std::shared_ptr<domain::interfaces::IProcessProvider> provider, QObject* parent)
    : QObject(parent), m_provider(std::move(provider)) {
    
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &ProcessViewModel::refresh);
    m_refreshTimer->start(2000); // 2 segundos (estilo htop default)
    
    refresh();
}

QVariantList ProcessViewModel::processes() const {
    QVariantList list;
    for (const auto& p : m_data) {
        if (!m_filter.isEmpty() && !p.command.contains(m_filter, Qt::CaseInsensitive) && !p.user.contains(m_filter, Qt::CaseInsensitive)) {
            continue;
        }
        
        QVariantMap map;
        map["pid"] = p.pid;
        map["user"] = p.user;
        map["priority"] = p.priority;
        map["nice"] = p.nice;
        map["virt"] = p.virt;
        map["res"] = p.res;
        map["cpu"] = p.cpuPercent;
        map["mem"] = p.memPercent;
        map["time"] = p.timeStr;
        map["command"] = p.command;
        
        QString stateStr;
        switch(p.state) {
            case domain::models::ProcessState::Running: stateStr = "R"; break;
            case domain::models::ProcessState::Sleeping: stateStr = "S"; break;
            case domain::models::ProcessState::DiskSleep: stateStr = "D"; break;
            case domain::models::ProcessState::Zombie: stateStr = "Z"; break;
            case domain::models::ProcessState::Stopped: stateStr = "T"; break;
            default: stateStr = "?"; break;
        }
        map["state"] = stateStr;
        
        list.append(map);
    }
    return list;
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
        emit sortFieldChanged();
        sortData();
        emit processesChanged();
    }
}

void ProcessViewModel::setSortAscending(bool asc) {
    if (m_sortAscending != asc) {
        m_sortAscending = asc;
        emit sortAscendingChanged();
        sortData();
        emit processesChanged();
    }
}

void ProcessViewModel::refresh() {
    m_data = m_provider->collect_processes();
    sortData();
    emit processesChanged();
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
    std::sort(m_data.begin(), m_data.end(), [this](const auto& a, const auto& b) {
        bool result = false;
        if (m_sortField == "cpu") result = a.cpuPercent < b.cpuPercent;
        else if (m_sortField == "mem") result = a.memPercent < b.memPercent;
        else if (m_sortField == "pid") result = a.pid < b.pid;
        else if (m_sortField == "user") result = a.user < b.user;
        else if (m_sortField == "virt") result = a.virt < b.virt;
        else if (m_sortField == "res") result = a.res < b.res;
        else if (m_sortField == "priority") result = a.priority < b.priority;
        else result = a.cpuPercent < b.cpuPercent;
        
        return m_sortAscending ? result : !result;
    });
}

}
