#pragma once
#include "../models/process_info.h"
#include <vector>

namespace my_comp::domain::interfaces {

class IProcessProvider {
public:
    virtual ~IProcessProvider() = default;
    
    virtual std::vector<models::ProcessInfo> collect_processes() = 0;
    virtual bool kill_process(int pid, int signal = 15) = 0; // 15 = SIGTERM, 9 = SIGKILL
    virtual bool set_priority(int pid, int nice) = 0;
};

} // namespace my_comp::domain::interfaces
