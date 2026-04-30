#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"
#include "infrastructure/collectors/vram/nvml_adapter.h"

#include <QString>

#include <functional>
#include <memory>

namespace vramopt::infrastructure {

struct DepsFactoryGpuBackend {
    std::function<std::shared_ptr<INvmlAdapter>()> criar_adapter_nvml;
    std::function<bool(const QString&)>            sistema_tem_intel;
    QString                                         drm_root  = QStringLiteral("/sys/class/drm");
    QString                                         proc_root = QStringLiteral("/proc");
};

[[nodiscard]] std::shared_ptr<domain::IGpuBackend>
    criarBackendCompositoCom(const DepsFactoryGpuBackend& deps);

[[nodiscard]] std::shared_ptr<domain::IGpuBackend>
    criarBackendCompositoDoSistema();

}
