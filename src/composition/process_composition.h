#pragma once

#include "composition/shared_infrastructure.h"
#include "presentation/viewmodels/dev_stack_viewmodel.h"
#include "presentation/viewmodels/process_viewmodel.h"
#include "presentation/viewmodels/service_viewmodel.h"

#include <memory>

namespace my_comp::composition {

// Processos, serviços systemd e stack de desenvolvimento (toolchains/containers).
struct ProcessViewModels {
    std::unique_ptr<presentation::viewmodels::ProcessViewModel> process;
    std::unique_ptr<presentation::viewmodels::ServiceViewModel> service;
    std::unique_ptr<presentation::viewmodels::DevStackViewModel> dev_stack;
};

[[nodiscard]] ProcessViewModels compor_processos(const SharedInfrastructure& infra);

}
