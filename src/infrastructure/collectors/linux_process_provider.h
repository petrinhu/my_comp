#pragma once
#include "../../domain/interfaces/i_process_provider.h"
#include <memory>
#include <map>
#include <chrono>
#include <mutex>

namespace my_comp::infrastructure::collectors {

struct ProcessTimeSample {
    unsigned long long utime;
    unsigned long long stime;
    unsigned long long total_time;
    double uptime;
};

class LinuxProcessProvider : public domain::interfaces::IProcessProvider {
public:
    LinuxProcessProvider();
    
    std::vector<domain::models::ProcessInfo> collect_processes() override;
    bool kill_process(int pid, int signal = 15) override;
    bool set_priority(int pid, int nice) override;

private:
    std::map<int, ProcessTimeSample> m_last_samples;
    double m_last_uptime = 0.0;
    long m_hertz = 100;
    unsigned long long m_total_ram = 0;
    mutable std::mutex m_mutex;

    void update_samples();
    double get_system_uptime();
    unsigned long long get_total_ram();
    QString get_user_from_uid(unsigned int uid);
};

} // namespace my_comp::infrastructure::collectors
