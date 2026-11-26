#include "linux_process_provider.h"
#include "numeric_parse.h"
#include "proc_cmdline.h"
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <pwd.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <QDateTime>
#include <QDebug>

namespace fs = std::filesystem;

namespace my_comp::infrastructure::collectors {

LinuxProcessProvider::LinuxProcessProvider() {
    m_hertz = sysconf(_SC_CLK_TCK);
    m_total_ram = get_total_ram();
    m_last_uptime = get_system_uptime();
}

double LinuxProcessProvider::get_system_uptime() {
    std::ifstream uptime_file("/proc/uptime");
    if (!uptime_file.is_open()) {
        qWarning() << "[Process] Failed to open /proc/uptime";
        return 0.0;
    }
    double uptime = 0.0;
    uptime_file >> uptime;
    return uptime;
}

unsigned long long LinuxProcessProvider::get_total_ram() {
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        qWarning() << "[Process] Failed to open /proc/meminfo";
        return 0;
    }
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 8, "MemTotal") == 0) {
            unsigned long long total = 0;
            std::stringstream ss(line.substr(10));
            ss >> total;
            return total * 1024;
        }
    }
    return 0;
}

QString LinuxProcessProvider::get_user_from_uid(unsigned int uid) {
    struct passwd pwd;
    struct passwd *result;
    char buffer[1024];
    // Retorno de getpwuid_r ignorado: validação vem via result == nullptr (POSIX).
    (void)getpwuid_r(uid, &pwd, buffer, sizeof(buffer), &result);
    if (result == nullptr) {
        return QString::number(uid);
    }
    return QString::fromLocal8Bit(pwd.pw_name);
}

std::vector<domain::models::ProcessInfo> LinuxProcessProvider::collect_processes() {
    const std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<domain::models::ProcessInfo> processes;
    const double current_uptime = get_system_uptime();
    const double delta_uptime = current_uptime - m_last_uptime;
    
    // Conjunto de PIDs ativos nesta rodada para limpeza do cache
    std::vector<int> active_pids;

    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            if (!entry.is_directory()) continue;
            
            std::string filename = entry.path().filename().string();
            const auto pid_lido = parse_integer<int>(filename);
            if (!pid_lido || *pid_lido <= 0) continue;
            const int pid = *pid_lido;
            active_pids.push_back(pid);

            const std::string stat_path = entry.path().string() + "/stat";
            std::ifstream stat_file(stat_path);
            if (!stat_file.is_open()) continue;
            
            std::string line;
            if (!std::getline(stat_file, line) || line.empty()) continue;
            
            // Parsing Robusto: (pid) (comm) state ppid ...
            const size_t open_paren = line.find('(');
            const size_t close_paren = line.find_last_of(')');
            if (open_paren == std::string::npos || close_paren == std::string::npos || close_paren <= open_paren) continue;
            
            domain::models::ProcessInfo p;
            p.pid = pid;
            // comm é cortado em 15 caracteres; cmdline traz o nome completo e os argumentos (AUDIT-30).
            const QString comm = QString::fromStdString(line.substr(open_paren + 1, close_paren - open_paren - 1));
            p.command = read_cmdline(QString::fromStdString((entry.path() / "cmdline").string()), comm);
            
            const std::string rest = line.substr(close_paren + 2);
            std::stringstream ss(rest);
            
            char state_char;
            int ppid;
            long long cutime, cstime, priority, nice, num_threads, itrealvalue;
            unsigned long long utime, stime, starttime;
            unsigned long vsize;
            long rss;
            unsigned int flags;
            unsigned long minflt, cminflt, majflt, cmajflt;
            int pgrp, session, tty_nr, tpgid;

            if (!(ss >> state_char >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags 
                   >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime)) continue;

            p.ppid = ppid;
            switch(state_char) {
                case 'R': p.state = domain::models::ProcessState::Running; break;
                case 'S': p.state = domain::models::ProcessState::Sleeping; break;
                case 'D': p.state = domain::models::ProcessState::DiskSleep; break;
                case 'Z': p.state = domain::models::ProcessState::Zombie; break;
                case 'T': p.state = domain::models::ProcessState::Stopped; break;
                default: p.state = domain::models::ProcessState::Unknown; break;
            }

            // CPU%
            const unsigned long long current_total_time = utime + stime;
            double cpu_usage = 0.0;
            if (m_last_samples.count(pid) && delta_uptime > 0) {
                const unsigned long long delta_time = current_total_time - m_last_samples[pid].total_time;
                cpu_usage = 100.0 * (static_cast<double>(delta_time) / static_cast<double>(m_hertz)) / delta_uptime;
            }
            m_last_samples[pid] = {utime, stime, current_total_time, current_uptime};

            // Resto dos dados
            if (ss >> cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >> starttime >> vsize >> rss) {
                p.priority = static_cast<int>(priority);
                p.nice = static_cast<int>(nice);
                p.virt = vsize / 1024;
                p.res = rss * sysconf(_SC_PAGESIZE) / 1024;
                p.cpuPercent = cpu_usage;
                p.memPercent = m_total_ram > 0 ? (100.0 * static_cast<double>(p.res) * 1024.0 / static_cast<double>(m_total_ram)) : 0.0;

                // Tempo
                const long long seconds = static_cast<long long>(current_uptime) - static_cast<long long>(starttime / m_hertz);
                p.timeStr = QString("%1:%2:%3")
                    .arg(seconds / 3600, 2, 10, QChar('0'))
                    .arg((seconds % 3600) / 60, 2, 10, QChar('0'))
                    .arg(seconds % 60, 2, 10, QChar('0'));
            }

            // Usuario (Opcional, se falhar mantem nulo/vazio)
            const std::string status_path = entry.path().string() + "/status";
            std::ifstream status_file(status_path);
            std::string s_line;
            while (std::getline(status_file, s_line)) {
                if (s_line.compare(0, 4, "Uid:") == 0) {
                    unsigned int uid;
                    std::stringstream ss_uid(s_line.substr(5));
                    if (ss_uid >> uid) p.user = get_user_from_uid(uid);
                    break;
                }
            }

            processes.push_back(p);
        }
    } catch (...) {
        qWarning() << "[Process] Error iterating /proc";
    }
    
    // Limpeza do Cache (Remover PIDs que não existem mais)
    if (m_last_samples.size() > processes.size() + 100) {
        auto it = m_last_samples.begin();
        while (it != m_last_samples.end()) {
            if (std::find(active_pids.begin(), active_pids.end(), it->first) == active_pids.end()) {
                it = m_last_samples.erase(it);
            } else {
                ++it;
            }
        }
    }

    m_last_uptime = current_uptime;
    return processes;
}

namespace {
// pid <= 0 endereça grupos (0 = grupo do próprio app, -1 = todos os processos do usuário,
// -N = grupo N); pid 1 é o init. O próprio my_comp também fica de fora (MC-10).
bool is_targetable_pid(int pid) {
    return pid > 1 && pid != static_cast<int>(::getpid());
}
constexpr int MIN_NICE = -20;
constexpr int MAX_NICE = 19;
} // namespace

bool LinuxProcessProvider::kill_process(int pid, int signal) {
    if (!is_targetable_pid(pid) || signal < 0 || signal >= NSIG) {
        return false;
    }
    return ::kill(pid, signal) == 0;
}

bool LinuxProcessProvider::set_priority(int pid, int nice) {
    if (!is_targetable_pid(pid) || nice < MIN_NICE || nice > MAX_NICE) {
        return false;
    }
    return ::setpriority(PRIO_PROCESS, static_cast<id_t>(pid), nice) == 0;
}

} // namespace my_comp::infrastructure::collectors
