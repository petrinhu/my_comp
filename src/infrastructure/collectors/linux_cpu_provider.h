#pragma once
#include "../../domain/interfaces/i_cpu_provider.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace my_comp::infrastructure::collectors {

struct CpuTime {
    long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    
    long long total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    
    long long active() const {
        return user + nice + system + irq + softirq + steal;
    }
};

class LinuxCpuProvider : public domain::interfaces::ICpuProvider {
public:
    LinuxCpuProvider();
    
    domain::models::CpuStats collect_stats() override;
    domain::models::CpuArchitecture collect_architecture() override;

private:
    std::vector<CpuTime> m_last_samples;
    
    std::vector<CpuTime> read_proc_stat();
    double read_temperature();
    double read_freq_khz(int cpu_id);
    void update_load(domain::models::CpuStats& stats);
};

} // namespace my_comp::infrastructure::collectors
