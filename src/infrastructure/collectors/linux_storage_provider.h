#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "../../domain/interfaces/i_storage_provider.h"

namespace my_comp::infrastructure::collectors {

struct DiskSample {
    uint64_t sectors_read;
    uint64_t sectors_written;
    std::chrono::steady_clock::time_point timestamp;
};

class LinuxStorageProvider : public domain::interfaces::IStorageProvider {
public:
    LinuxStorageProvider();
    domain::models::StorageStats collect_stats() override;

private:
    std::map<std::string, DiskSample> m_last_samples;
};

} // namespace my_comp::infrastructure::collectors
