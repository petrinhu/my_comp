#pragma once
#include "../../domain/interfaces/i_system_info_provider.h"
#include "../../infrastructure/collectors/i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class LinuxSystemInfoProvider : public domain::interfaces::ISystemInfoProvider {
public:
    explicit LinuxSystemInfoProvider(std::shared_ptr<IShellExecutor> shell);
    domain::models::SystemInfo get_system_info() override;

private:
    std::shared_ptr<IShellExecutor> m_shell;
};

} // namespace my_comp::infrastructure::collectors
