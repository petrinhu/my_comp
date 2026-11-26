#pragma once

// Composition root da GUI (MC-02: DI manual). Cada feature tem seu módulo compor_<feature>();
// este arquivo só agrega e publica os ViewModels no QML.

#include "composition/process_composition.h"
#include "composition/storage_composition.h"
#include "composition/system_composition.h"
#include "composition/updates_composition.h"
#include "presentation/viewmodels/audio_viewmodel.h"
#include "presentation/viewmodels/export_viewmodel.h"
#include "presentation/viewmodels/vram_audit_viewmodel.h"

#include <memory>

class QQmlContext;

namespace my_comp::composition {

struct AppViewModels {
    SystemViewModels system;
    StorageViewModels storage;
    ProcessViewModels process;
    UpdatesViewModels updates;
    std::unique_ptr<presentation::viewmodels::AudioViewModel> audio;
    std::unique_ptr<presentation::viewmodels::ExportViewModel> exporter;
    std::unique_ptr<my_comp::presentation::viewmodels::VramAuditViewModel> vram_audit;
};

[[nodiscard]] AppViewModels compor_app();

// Publica cada ViewModel com o nome de context property que o QML espera.
void registrar_no_qml(QQmlContext& ctx, const AppViewModels& vms);

}
