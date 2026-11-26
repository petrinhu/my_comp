#pragma once
#include "../models/process_info.h"
#include <cstdint>
#include <vector>

namespace my_comp::domain::interfaces {

class IProcessProvider {
public:
    virtual ~IProcessProvider() = default;
    
    virtual std::vector<models::ProcessInfo> collect_processes() = 0;
    // Só sinaliza se o processo em `pid` ainda tem `expected_start_time` (o da coleta):
    // PID reciclado entre a listagem e a confirmação é recusado. 15 = SIGTERM, 9 = SIGKILL.
    virtual bool kill_process(int pid, std::uint64_t expected_start_time, int signal = 15) = 0;
    virtual bool set_priority(int pid, int nice) = 0;
};

} // namespace my_comp::domain::interfaces
