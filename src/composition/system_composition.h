#pragma once

#include "composition/shared_infrastructure.h"
#include "presentation/viewmodels/cpu_viewmodel.h"
#include "presentation/viewmodels/firmware_viewmodel.h"
#include "presentation/viewmodels/gpu_viewmodel.h"
#include "presentation/viewmodels/hardware_viewmodel.h"
#include "presentation/viewmodels/memory_viewmodel.h"
#include "presentation/viewmodels/network_viewmodel.h"
#include "presentation/viewmodels/power_viewmodel.h"
#include "presentation/viewmodels/system_info_viewmodel.h"
#include "presentation/viewmodels/user_metadata_viewmodel.h"

#include <memory>

namespace my_comp::composition {

// Monitores de sistema: CPU, memória, firmware, GPU, energia, rede, identidade e hardware.
struct SystemViewModels {
    std::unique_ptr<presentation::viewmodels::CpuViewModel> cpu;
    std::unique_ptr<presentation::viewmodels::MemoryViewModel> memory;
    std::unique_ptr<presentation::viewmodels::FirmwareViewModel> firmware;
    std::unique_ptr<presentation::viewmodels::GpuViewModel> gpu;
    std::unique_ptr<presentation::viewmodels::PowerViewModel> power;
    std::unique_ptr<presentation::viewmodels::NetworkViewModel> network;
    std::unique_ptr<presentation::viewmodels::SystemInfoViewModel> system_info;
    std::unique_ptr<presentation::viewmodels::UserMetadataViewModel> user_metadata;
    std::unique_ptr<presentation::viewmodels::HardwareViewModel> hardware;
};

[[nodiscard]] SystemViewModels compor_sistema(const SharedInfrastructure& infra);

}
