#pragma once

#include "composition/shared_infrastructure.h"
#include "presentation/viewmodels/diagnostic_viewmodel.h"
#include "presentation/viewmodels/installation_viewmodel.h"
#include "presentation/viewmodels/update_viewmodel.h"

#include <memory>

namespace my_comp::composition {

// Atualizações (multi-PM detectado em runtime), instalação e diagnóstico do journal.
struct UpdatesViewModels {
    std::unique_ptr<presentation::viewmodels::UpdateViewModel> update;
    std::unique_ptr<presentation::viewmodels::InstallationViewModel> installation;
    std::unique_ptr<presentation::viewmodels::DiagnosticViewModel> diagnostic;
};

[[nodiscard]] UpdatesViewModels compor_atualizacoes(const SharedInfrastructure& infra);

}
