#pragma once
#include <memory>
#include "../../domain/interfaces/i_smart_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

class LinuxSmartProvider : public domain::interfaces::ISmartProvider {
public:
    explicit LinuxSmartProvider(std::shared_ptr<IShellExecutor> shell)
        : m_shell(std::move(shell)) {}

    domain::models::SmartReport collect_report(const QString& device) override;

private:
    std::shared_ptr<IShellExecutor> m_shell;
};

} // namespace my_comp::infrastructure::collectors
