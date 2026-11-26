#include "composition/system_composition.h"

#include "infrastructure/collectors/dbus_firmware_provider.h"
#include "infrastructure/collectors/linux_cpu_provider.h"
#include "infrastructure/collectors/linux_hardware_provider.h"
#include "infrastructure/collectors/linux_memory_provider.h"
#include "infrastructure/collectors/linux_network_provider.h"
#include "infrastructure/collectors/linux_power_provider.h"
#include "infrastructure/collectors/linux_system_info_provider.h"
#include "infrastructure/collectors/linux_user_metadata_provider.h"
#include "infrastructure/collectors/nvidia_gpu_provider.h"
#include "infrastructure/collectors/sysfs_gpu_provider.h"

namespace my_comp::composition {

namespace vm = presentation::viewmodels;
namespace col = infrastructure::collectors;

namespace {

// nvidia-smi quando presente; senão sysfs (genérico AMD/Intel).
std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> compor_gpu_providers(const SharedInfrastructure& infra) {
    std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> providers;
    if (ferramenta_disponivel(QStringLiteral("nvidia-smi"))) {
        providers.push_back(std::make_unique<col::NvidiaGpuProvider>(infra.shell));
    } else {
        providers.push_back(std::make_unique<col::SysfsGpuProvider>());
    }
    return providers;
}

} // namespace

SystemViewModels compor_sistema(const SharedInfrastructure& infra) {
    SystemViewModels vms;
    vms.cpu = std::make_unique<vm::CpuViewModel>(std::make_shared<col::LinuxCpuProvider>());
    vms.memory = std::make_unique<vm::MemoryViewModel>(std::make_shared<col::LinuxMemoryProvider>());
    vms.firmware = std::make_unique<vm::FirmwareViewModel>(std::make_shared<col::DBusFirmwareProvider>());
    vms.gpu = std::make_unique<vm::GpuViewModel>(compor_gpu_providers(infra));
    vms.power = std::make_unique<vm::PowerViewModel>(std::make_shared<col::LinuxPowerProvider>());
    vms.network = std::make_unique<vm::NetworkViewModel>(std::make_shared<col::LinuxNetworkProvider>(infra.shell), infra.preferences);
    vms.system_info = std::make_unique<vm::SystemInfoViewModel>(std::make_shared<col::LinuxSystemInfoProvider>(infra.shell));
    vms.user_metadata = std::make_unique<vm::UserMetadataViewModel>(std::make_shared<col::LinuxUserMetadataProvider>(infra.shell));
    vms.hardware = std::make_unique<vm::HardwareViewModel>(std::make_shared<col::LinuxHardwareProvider>(infra.shell));
    return vms;
}

}
