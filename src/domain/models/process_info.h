#pragma once
#include <QString>
#include <vector>

namespace my_comp::domain::models {

enum class ProcessState {
    Running,
    Sleeping,
    DiskSleep,
    Zombie,
    Stopped,
    Tracing,
    Unknown
};

struct ProcessInfo {
    int pid;
    int ppid;
    QString user;
    int priority;
    int nice;
    unsigned long virt; // KB
    unsigned long res;  // KB
    unsigned long shr;  // KB
    ProcessState state;
    double cpuPercent;
    double memPercent;
    QString timeStr;
    QString command;
    
    // Para Tree View
    std::vector<int> children;
};

} // namespace my_comp::domain::models
