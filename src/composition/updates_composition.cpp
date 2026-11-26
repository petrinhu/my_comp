#include "composition/updates_composition.h"

#include "application/diagnostic_manager.h"
#include "application/update_manager.h"
#include "infrastructure/collectors/apt_update_provider.h"
#include "infrastructure/collectors/cargo_update_provider.h"
#include "infrastructure/collectors/dnf_update_provider.h"
#include "infrastructure/collectors/flatpak_update_provider.h"
#include "infrastructure/collectors/installer_adapter.h"
#include "infrastructure/collectors/interactive_process_runner.h"
#include "infrastructure/collectors/journal_diagnostic_provider.h"
#include "infrastructure/collectors/npm_update_provider.h"
#include "infrastructure/collectors/pacman_update_provider.h"
#include "infrastructure/collectors/snap_update_provider.h"

namespace my_comp::composition {

namespace vm = presentation::viewmodels;
namespace col = infrastructure::collectors;

namespace {

// Um provider por gerenciador de pacotes presente no sistema (design agnóstico a distro).
void registrar_gerenciadores(application::UpdateManager& manager, const SharedInfrastructure& infra) {
    const auto& shell = infra.shell;
    if (ferramenta_disponivel(QStringLiteral("dnf"))) manager.register_provider(std::make_unique<col::DnfUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("apt"))) manager.register_provider(std::make_unique<col::AptUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("pacman"))) manager.register_provider(std::make_unique<col::PacmanUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("flatpak"))) manager.register_provider(std::make_unique<col::FlatpakUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("snap"))) manager.register_provider(std::make_unique<col::SnapUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("npm"))) manager.register_provider(std::make_unique<col::NpmUpdateProvider>(shell));
    if (ferramenta_disponivel(QStringLiteral("cargo"))) manager.register_provider(std::make_unique<col::CargoUpdateProvider>(shell));
}

} // namespace

UpdatesViewModels compor_atualizacoes(const SharedInfrastructure& infra) {
    auto installer = std::make_shared<col::InstallerAdapter>(std::make_shared<col::InteractiveProcessRunner>());

    auto update_manager = std::make_shared<application::UpdateManager>();
    update_manager->set_installer(installer);
    registrar_gerenciadores(*update_manager, infra);

    auto diagnostic_manager = std::make_shared<application::DiagnosticManager>();
    diagnostic_manager->register_provider(std::make_unique<col::JournalDiagnosticProvider>(infra.shell));

    UpdatesViewModels vms;
    vms.update = std::make_unique<vm::UpdateViewModel>(update_manager);
    vms.installation = std::make_unique<vm::InstallationViewModel>(installer);
    vms.diagnostic = std::make_unique<vm::DiagnosticViewModel>(diagnostic_manager);
    return vms;
}

}
