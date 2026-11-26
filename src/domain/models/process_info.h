#pragma once
#include <QString>
#include <cstdint>
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
    int pid = 0;
    int ppid = 0;
    QString user;
    int priority = 0;
    int nice = 0;
    unsigned long virt = 0; // KB
    unsigned long res = 0;  // KB
    unsigned long shr = 0;  // KB
    ProcessState state = ProcessState::Unknown;
    double cpuPercent = 0.0;
    double memPercent = 0.0;
    QString timeStr;
    QString command;
    // starttime de /proc/<pid>/stat (ticks desde o boot): com o PID, identifica o
    // processo mesmo que o kernel recicle o número (kill seguro).
    std::uint64_t start_time = 0;

    std::vector<int> children;
};

} // namespace my_comp::domain::models
