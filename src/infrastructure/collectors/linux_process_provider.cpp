#include "linux_process_provider.h"
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <pwd.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <QDateTime>
#include <QRegularExpression>

namespace fs = std::filesystem;

namespace my_comp::infrastructure::collectors {

LinuxProcessProvider::LinuxProcessProvider() {
    m_hertz = sysconf(_SC_CLK_TCK);
    m_total_ram = get_total_ram();
    m_last_uptime = get_system_uptime();
}

double LinuxProcessProvider::get_system_uptime() {
    std::ifstream uptime_file("/proc/uptime");
    double uptime;
    if (uptime_file >> uptime) return uptime;
    return 0.0;
}

unsigned long long LinuxProcessProvider::get_total_ram() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 8, "MemTotal") == 0) {
            unsigned long long total;
            std::stringstream ss(line.substr(10));
            ss >> total;
            return total * 1024; // KB to B
        }
    }
    return 0;
}

QString LinuxProcessProvider::get_user_from_uid(unsigned int uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) return QString::fromLocal8Bit(pw->pw_name);
    return QString::number(uid);
}

std::vector<domain::models::ProcessInfo> LinuxProcessProvider::collect_processes() {
    std::vector<domain::models::ProcessInfo> processes;
    double current_uptime = get_system_uptime();
    double delta_uptime = current_uptime - m_last_uptime;
    
    for (const auto& entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory()) continue;
        
        std::string filename = entry.path().filename().string();
        if (!std::all_of(filename.begin(), filename.end(), ::isdigit)) continue;
        
        int pid = std::stoi(filename);
        std::string stat_path = entry.path().string() + "/stat";
        std::ifstream stat_file(stat_path);
        if (!stat_file.is_open()) continue;
        
        std::string line;
        std::getline(stat_file, line);
        
        // Formato stat: (1)pid (2)comm (3)state (4)ppid (5)pgrp (6)session (7)tty_nr (8)tpgid (9)flags (10)minflt (11)cminflt (12)majflt (13)cmajflt (14)utime (15)stime
        // Nota: (comm) pode conter espacos, usamos regex ou busca manual do ultimo ')'
        size_t last_paren = line.find_last_of(')');
        if (last_paren == std::string::npos) continue;
        
        std::string pid_and_comm = line.substr(0, last_paren + 1);
        std::string rest = line.substr(last_paren + 2);
        std::stringstream ss(rest);
        
        char state_char;
        int ppid, pgrp, session, tty_nr, tpgid;
        unsigned int flags;
        unsigned long minflt, cminflt, majflt, cmajflt;
        unsigned long long utime, stime;
        long long cutime, cstime, priority, nice, num_threads, itrealvalue;
        unsigned long long starttime;
        unsigned long vsize;
        long rss;
        
        ss >> state_char >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags 
           >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime;
           
        // Dados para CPU%
        unsigned long long current_total_time = utime + stime;
        double cpu_usage = 0.0;
        if (m_last_samples.count(pid) && delta_uptime > 0) {
            unsigned long long delta_time = current_total_time - m_last_samples[pid].total_time;
            cpu_usage = 100.0 * ((double)delta_time / m_hertz) / delta_uptime;
        }
        m_last_samples[pid] = {utime, stime, current_total_time, current_uptime};

        domain::models::ProcessInfo p;
        p.pid = pid;
        p.ppid = ppid;
        p.command = QString::fromStdString(pid_and_comm.substr(line.find('(') + 1, last_paren - line.find('(') - 1));
        
        switch(state_char) {
            case 'R': p.state = domain::models::ProcessState::Running; break;
            case 'S': p.state = domain::models::ProcessState::Sleeping; break;
            case 'D': p.state = domain::models::ProcessState::DiskSleep; break;
            case 'Z': p.state = domain::models::ProcessState::Zombie; break;
            case 'T': p.state = domain::models::ProcessState::Stopped; break;
            default: p.state = domain::models::ProcessState::Unknown; break;
        }

        // Mais campos apos utime/stime
        ss >> cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >> starttime >> vsize >> rss;
        
        p.priority = (int)priority;
        p.nice = (int)nice;
        p.virt = vsize / 1024;
        p.res = rss * sysconf(_SC_PAGESIZE) / 1024;
        p.cpuPercent = cpu_usage;
        p.memPercent = m_total_ram > 0 ? (100.0 * p.res * 1024 / m_total_ram) : 0.0;
        
        // Usuario
        fs::path status_path = entry.path() / "status";
        std::ifstream status_file(status_path);
        std::string s_line;
        while (std::getline(status_file, s_line)) {
            if (s_line.compare(0, 4, "Uid:") == 0) {
                unsigned int uid;
                std::stringstream ss_uid(s_line.substr(5));
                ss_uid >> uid;
                p.user = get_user_from_uid(uid);
                break;
            }
        }

        // Tempo de execucao aproximado
        long long seconds = (long long)current_uptime - (starttime / m_hertz);
        p.timeStr = QString("%1:%2:%3")
            .arg(seconds / 3600, 2, 10, QChar('0'))
            .arg((seconds % 3600) / 60, 2, 10, QChar('0'))
            .arg(seconds % 60, 2, 10, QChar('0'));

        processes.push_back(p);
    }
    
    m_last_uptime = current_uptime;
    return processes;
}

bool LinuxProcessProvider::kill_process(int pid, int signal) {
    return ::kill(pid, signal) == 0;
}

bool LinuxProcessProvider::set_priority(int pid, int nice) {
    return ::setpriority(PRIO_PROCESS, pid, nice) == 0;
}

} // namespace my_comp::infrastructure::collectors
