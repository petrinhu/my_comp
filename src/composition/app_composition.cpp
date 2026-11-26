#include "composition/app_composition.h"

#include "composition/vram_composition.h"
#include "infrastructure/audio/qt_audio_provider.h"
#include "infrastructure/exporters/html_export_provider.h"
#include "infrastructure/exporters/json_export_provider.h"
#include "infrastructure/exporters/markdown_export_provider.h"

#include <QQmlContext>

namespace my_comp::composition {

namespace vm = presentation::viewmodels;

namespace {

// O export lê snapshots de outros VMs (observação via QPointer, sem ownership).
std::unique_ptr<vm::ExportViewModel> compor_export(const SharedInfrastructure& infra, const SystemViewModels& sys,
                                                  const ProcessViewModels& proc) {
    auto exporter = std::make_unique<vm::ExportViewModel>(std::make_shared<application::ExportHistory>(infra.preferences));
    exporter->add_provider(std::make_unique<infrastructure::exporters::JsonExportProvider>());
    exporter->add_provider(std::make_unique<infrastructure::exporters::MarkdownExportProvider>());
    exporter->add_provider(std::make_unique<infrastructure::exporters::HtmlExportProvider>());
    exporter->set_vms(sys.system_info.get(), sys.cpu.get(), sys.memory.get(),
                      sys.user_metadata.get(), sys.hardware.get(), proc.dev_stack.get());
    return exporter;
}

} // namespace

AppViewModels compor_app() {
    const auto infra = compor_infra_compartilhada();

    AppViewModels vms;
    vms.system = compor_sistema(infra);
    vms.storage = compor_armazenamento(infra);
    vms.process = compor_processos(infra);
    vms.updates = compor_atualizacoes(infra);
    vms.audio = std::make_unique<vm::AudioViewModel>(std::make_shared<infrastructure::audio::QtAudioProvider>());
    vms.exporter = compor_export(infra, vms.system, vms.process);
    auto servicos_vram = my_comp::composition::compor_servicos_vram();
    vms.vram_audit = std::make_unique<my_comp::presentation::viewmodels::VramAuditViewModel>(
        std::move(servicos_vram.auditar), std::move(servicos_vram.registrar_feedback));
    vms.url_helper = std::make_unique<vm::UrlHelper>();
    return vms;
}

void registrar_no_qml(QQmlContext& ctx, const AppViewModels& vms) {
    ctx.setContextProperty("cpuViewModel", vms.system.cpu.get());
    ctx.setContextProperty("memoryViewModel", vms.system.memory.get());
    ctx.setContextProperty("firmwareViewModel", vms.system.firmware.get());
    ctx.setContextProperty("gpuViewModel", vms.system.gpu.get());
    ctx.setContextProperty("powerViewModel", vms.system.power.get());
    ctx.setContextProperty("networkViewModel", vms.system.network.get());
    ctx.setContextProperty("systemInfoViewModel", vms.system.system_info.get());
    ctx.setContextProperty("userMetadataViewModel", vms.system.user_metadata.get());
    ctx.setContextProperty("hardwareViewModel", vms.system.hardware.get());

    ctx.setContextProperty("storageViewModel", vms.storage.storage.get());
    ctx.setContextProperty("directoryScannerViewModel", vms.storage.directory_scanner.get());

    ctx.setContextProperty("processViewModel", vms.process.process.get());
    ctx.setContextProperty("serviceViewModel", vms.process.service.get());
    ctx.setContextProperty("devStackViewModel", vms.process.dev_stack.get());

    ctx.setContextProperty("updateViewModel", vms.updates.update.get());
    ctx.setContextProperty("installationViewModel", vms.updates.installation.get());
    ctx.setContextProperty("diagnosticViewModel", vms.updates.diagnostic.get());

    ctx.setContextProperty("audioViewModel", vms.audio.get());
    ctx.setContextProperty("exportViewModel", vms.exporter.get());
    ctx.setContextProperty("vramAuditViewModel", vms.vram_audit.get());
    ctx.setContextProperty("urlHelper", vms.url_helper.get());
}

}
