#pragma once

#include "../../domain/interfaces/i_installer.h"
#include "../../domain/interfaces/i_interactive_runner.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class InstallerAdapter : public domain::interfaces::IInstaller {
public:
    explicit InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner);
    void install(const domain::models::UpdatePackage& package) override;

private:
    std::shared_ptr<domain::interfaces::IInteractiveRunner> m_runner;
};

} // namespace my_comp::infrastructure::collectors
