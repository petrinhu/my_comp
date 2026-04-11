#include "linux_storage_provider.h"
#include <sstream>
#include <algorithm>

namespace my_comp::infrastructure::collectors {

LinuxStorageProvider::LinuxStorageProvider() {
    std::ifstream diskstats("/proc/diskstats");
    std::string line;
    auto now = std::chrono::steady_clock::now();
    while (std::getline(diskstats, line)) {
        std::stringstream ss(line);
        uint32_t major, minor; std::string dev_name;
        uint64_t reads, reads_merged, sectors_read, time_read, writes, writes_merged, sectors_written, time_write;
        ss >> major >> minor >> dev_name >> reads >> reads_merged >> sectors_read >> time_read >> writes >> writes_merged >> sectors_written >> time_write;
        m_last_samples[dev_name] = {sectors_read, sectors_written, now};
    }
}

domain::models::StorageStats LinuxStorageProvider::collect_stats() {
    domain::models::StorageStats stats;
    std::ifstream diskstats("/proc/diskstats");
    std::string line;
    auto now = std::chrono::steady_clock::now();
    
    while (std::getline(diskstats, line)) {
        std::stringstream ss(line);
        uint32_t major, minor;
        std::string dev_name;
        uint64_t reads, reads_merged, sectors_read, time_read;
        uint64_t writes, writes_merged, sectors_written, time_write;
        
        ss >> major >> minor >> dev_name >> reads >> reads_merged >> sectors_read >> time_read 
           >> writes >> writes_merged >> sectors_written >> time_write;
        
        // Filtro básico para discos físicos (nvmeXn1, sdX)
        bool is_physical = (dev_name.compare(0, 4, "nvme") == 0 && dev_name.find('p') == std::string::npos) ||
                           (dev_name.compare(0, 2, "sd") == 0 && dev_name.size() == 3);
        
        if (!is_physical) continue;

        if (m_last_samples.count(dev_name)) {
            const auto& last = m_last_samples[dev_name];
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last.timestamp).count();
            
            if (duration > 0) {
                double seconds = duration / 1000.0;
                // Setores no Linux são tipicamente 512 bytes
                double read_mb = (sectors_read - last.sectors_read) * 512.0 / (1024.0 * 1024.0);
                double write_mb = (sectors_written - last.sectors_written) * 512.0 / (1024.0 * 1024.0);
                
                domain::models::DriveStat drive;
                drive.name = QString::fromStdString(dev_name);
                drive.read_mb_s = read_mb / seconds;
                drive.write_mb_s = write_mb / seconds;
                
                stats.drives.push_back(drive);
                stats.total_read_mb_s += drive.read_mb_s;
                stats.total_write_mb_s += drive.write_mb_s;
            }
        }
        
        m_last_samples[dev_name] = {sectors_read, sectors_written, now};
    }
    
    return stats;
}

} // namespace my_comp::infrastructure::collectors
