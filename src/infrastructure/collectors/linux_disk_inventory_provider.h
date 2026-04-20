#pragma once
#include <memory>
#include "../../domain/interfaces/i_disk_inventory_provider.h"
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

class LinuxDiskInventoryProvider : public domain::interfaces::IDiskInventoryProvider {
public:
    explicit LinuxDiskInventoryProvider(std::shared_ptr<IShellExecutor> shell)
        : m_shell(std::move(shell)) {}

    domain::models::DiskInventory collect_inventory() override;

private:
    std::shared_ptr<IShellExecutor> m_shell;
};

} // namespace my_comp::infrastructure::collectors
