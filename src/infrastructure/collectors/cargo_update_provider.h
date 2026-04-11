#pragma once
#include "../../domain/interfaces/i_update_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class CargoUpdateProvider : public domain::interfaces::IUpdateProvider {
public:
    explicit CargoUpdateProvider(std::shared_ptr<IShellExecutor> executor);
    std::vector<domain::models::UpdatePackage> check_updates() override;
    QString get_name() const override { return "Cargo"; }
    bool is_available() const override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
};

} // namespace my_comp::infrastructure::collectors
