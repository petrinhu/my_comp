#pragma once

#include "composition/shared_infrastructure.h"
#include "presentation/viewmodels/directory_scanner_viewmodel.h"
#include "presentation/viewmodels/storage_viewmodel.h"

#include <memory>

namespace my_comp::composition {

// Armazenamento: ocupação, inventário, SMART, benchmark e varredura de diretórios.
struct StorageViewModels {
    std::unique_ptr<presentation::viewmodels::StorageViewModel> storage;
    std::unique_ptr<presentation::viewmodels::DirectoryScannerViewModel> directory_scanner;
};

[[nodiscard]] StorageViewModels compor_armazenamento(const SharedInfrastructure& infra);

}
