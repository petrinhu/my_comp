#include "composition/process_composition.h"

#include "infrastructure/collectors/linux_dev_stack_provider.h"
#include "infrastructure/collectors/linux_process_provider.h"
#include "infrastructure/collectors/systemd_service_provider.h"

namespace my_comp::composition {

namespace vm = presentation::viewmodels;
namespace col = infrastructure::collectors;

ProcessViewModels compor_processos(const SharedInfrastructure& infra) {
    ProcessViewModels vms;
    vms.process = std::make_unique<vm::ProcessViewModel>(std::make_shared<col::LinuxProcessProvider>());
    vms.service = std::make_unique<vm::ServiceViewModel>(std::make_shared<col::SystemdServiceProvider>(infra.shell));
    vms.dev_stack = std::make_unique<vm::DevStackViewModel>(std::make_shared<col::LinuxDevStackProvider>(infra.shell));
    return vms;
}

}
