#include "linux_cpu_provider.h"
#include <filesystem>
#include <regex>
#include <iostream>

namespace fs = std::filesystem;

namespace my_comp::infrastructure::collectors {

LinuxCpuProvider::LinuxCpuProvider() {
    m_last_samples = read_proc_stat();
}

domain::models::CpuStats LinuxCpuProvider::collect_stats() {
    domain::models::CpuStats stats = {0.0, 0.0, 0.0, {}};
    update_load(stats);
    stats.temperature = read_temperature();
    
    double total_freq = 0;
    for (int i = 0; i < static_cast<int>(stats.cores.size()); ++i) {
        const double freq = read_freq_khz(i) / 1000.0;
        stats.cores[i].frequency_mhz = freq;
        total_freq += freq;
    }
    
    if (!stats.cores.empty()) {
        stats.average_frequency = total_freq / static_cast<double>(stats.cores.size());
    }
    
    return stats;
}

domain::models::CpuArchitecture LinuxCpuProvider::collect_architecture() {
    domain::models::CpuArchitecture arch = {"", "", 0, 0, {}, "", "", ""};
    
    // 1. Ler /proc/cpuinfo
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    const std::regex model_regex("^model name\\s+:\\s+(.*)$");
    const std::regex vendor_regex("^vendor_id\\s+:\\s+(.*)$");
    const std::regex cores_regex("^cpu cores\\s+:\\s+(.*)$");
    const std::regex threads_regex("^siblings\\s+:\\s+(.*)$");
    
    while (std::getline(cpuinfo, line)) {
        std::smatch match;
        if (std::regex_search(line, match, model_regex)) arch.model_name = match[1];
        if (std::regex_search(line, match, vendor_regex)) arch.vendor_id = match[1];
        if (std::regex_search(line, match, cores_regex)) arch.physical_cores = std::stoi(match[1]);
        if (std::regex_search(line, match, threads_regex)) arch.logical_threads = std::stoi(match[1]);
    }
    
    // 2. Ler Caches via /sys
    for (int i = 0; i < 4; ++i) {
        const std::string cache_path = "/sys/devices/system/cpu/cpu0/cache/index" + std::to_string(i);
        if (fs::exists(cache_path)) {
            domain::models::CacheInfo ci;
            std::ifstream level_file(cache_path + "/level");
            std::ifstream type_file(cache_path + "/type");
            std::ifstream size_file(cache_path + "/size");
            level_file >> ci.level;
            type_file >> ci.type;
            size_file >> ci.size;
            arch.caches.push_back(ci);
            
            if (ci.level == "1" && ci.type == "Data") arch.l1_cache = ci.size;
            if (ci.level == "2") arch.l2_cache = ci.size;
            if (ci.level == "3") arch.l3_cache = ci.size;
        }
    }
    
    return arch;
}

std::vector<CpuTime> LinuxCpuProvider::read_proc_stat() {
    std::vector<CpuTime> samples;
    std::ifstream stat("/proc/stat");
    std::string line;
    
    while (std::getline(stat, line)) {
        if (line.compare(0, 3, "cpu") == 0) {
            std::stringstream ss(line);
            std::string cpu_label;
            CpuTime t;
            ss >> cpu_label >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >> t.softirq >> t.steal >> t.guest >> t.guest_nice;
            samples.push_back(t);
        } else if (!line.empty()) {
            break;
        }
    }
    return samples;
}

double LinuxCpuProvider::read_temperature() {
    // Tenta encontrar uma zona térmica relevante
    for (int i = 0; i < 10; ++i) {
        const std::string path = "/sys/class/thermal/thermal_zone" + std::to_string(i) + "/temp";
        if (fs::exists(path)) {
            std::ifstream temp_file(path);
            long long temp_raw;
            if (temp_file >> temp_raw) return static_cast<double>(temp_raw) / 1000.0;
        }
    }
    return 0.0;
}

double LinuxCpuProvider::read_freq_khz(int cpu_id) {
    const std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu_id) + "/cpufreq/scaling_cur_freq";
    if (fs::exists(path)) {
        std::ifstream freq_file(path);
        double freq;
        if (freq_file >> freq) return freq;
    }
    return 0.0;
}

void LinuxCpuProvider::update_load(domain::models::CpuStats& stats) {
    auto current = read_proc_stat();
    if (current.size() != m_last_samples.size()) {
        m_last_samples = current;
        return;
    }
    
    for (size_t i = 0; i < current.size(); ++i) {
        const long long total_diff = current[i].total() - m_last_samples[i].total();
        const long long active_diff = current[i].active() - m_last_samples[i].active();
        
        const double load = (total_diff > 0) ? (100.0 * static_cast<double>(active_diff) / static_cast<double>(total_diff)) : 0.0;
        
        if (i == 0) {
            stats.total_load = load;
        } else {
            domain::models::CoreStat core = {0, 0.0, 0.0};
            core.id = static_cast<int>(i) - 1;
            core.load_percent = load;
            stats.cores.push_back(core);
        }
    }
    m_last_samples = current;
}

} // namespace my_comp::infrastructure::collectors
