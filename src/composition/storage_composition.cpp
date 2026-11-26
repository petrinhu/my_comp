#include "composition/storage_composition.h"

#include "infrastructure/collectors/filesystem_directory_scanner.h"
#include "infrastructure/collectors/hdparm_benchmark_provider.h"
#include "infrastructure/collectors/linux_disk_inventory_provider.h"
#include "infrastructure/collectors/linux_smart_provider.h"
#include "infrastructure/collectors/linux_storage_provider.h"

namespace my_comp::composition {

namespace vm = presentation::viewmodels;
namespace col = infrastructure::collectors;

StorageViewModels compor_armazenamento(const SharedInfrastructure& infra) {
    // SMART e benchmark são opcionais: sem a ferramenta, o VM recebe nullptr e esconde a seção.
    std::shared_ptr<domain::interfaces::ISmartProvider> smart;
    if (ferramenta_disponivel(QStringLiteral("smartctl"))) {
        smart = std::make_shared<col::LinuxSmartProvider>(infra.shell);
    }
    std::shared_ptr<domain::interfaces::IBenchmarkProvider> benchmark;
    if (ferramenta_disponivel(QStringLiteral("hdparm"))) {
        benchmark = std::make_shared<col::HdparmBenchmarkProvider>(infra.shell);
    }

    StorageViewModels vms;
    vms.storage = std::make_unique<vm::StorageViewModel>(
        std::make_shared<col::LinuxStorageProvider>(),
        std::make_shared<col::LinuxDiskInventoryProvider>(infra.shell),
        smart, benchmark);
    vms.directory_scanner = std::make_unique<vm::DirectoryScannerViewModel>(
        std::make_shared<col::FilesystemDirectoryScanner>());
    return vms;
}

}
