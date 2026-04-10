#pragma once
#include "../../domain/interfaces/i_hardware_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class LinuxHardwareProvider : public domain::interfaces::IHardwareProvider {
public:
    explicit LinuxHardwareProvider(std::shared_ptr<IShellExecutor> executor);
    domain::models::HardwareTopology collect_topology() override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
    
    void collect_smbios(domain::models::HardwareTopology& topology);
    void collect_usb(domain::models::HardwareTopology& topology);
    void collect_bluetooth(domain::models::HardwareTopology& topology);
};

} // namespace my_comp::infrastructure::collectors
