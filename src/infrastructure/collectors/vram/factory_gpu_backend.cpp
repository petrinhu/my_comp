#include "infrastructure/collectors/vram/factory_gpu_backend.h"

#include "infrastructure/collectors/vram/composite_gpu_backend.h"
#include "infrastructure/collectors/vram/intel_i915_gpu_backend.h"
#include "infrastructure/collectors/vram/null_gpu_backend.h"
#include "infrastructure/collectors/vram/nvml_adapter_dl.h"
#include "infrastructure/collectors/vram/nvml_gpu_backend.h"

#include <utility>
#include <vector>

namespace vramopt::infrastructure {

std::shared_ptr<domain::IGpuBackend>
criarBackendCompositoCom(const DepsFactoryGpuBackend& deps) {
    std::vector<std::shared_ptr<domain::IGpuBackend>> backends;

    if (deps.criar_adapter_nvml) {
        auto adapter = deps.criar_adapter_nvml();
        if (adapter && adapter->isAvailable()) {
            backends.push_back(std::make_shared<NvmlGpuBackend>(adapter, deps.proc_root));
        }
    }

    if (deps.sistema_tem_intel && deps.sistema_tem_intel(deps.drm_root)) {
        backends.push_back(std::make_shared<IntelI915GpuBackend>(deps.drm_root, deps.proc_root));
    }

    if (backends.empty()) {
        return std::make_shared<NullGpuBackend>();
    }
    return std::make_shared<CompositeGpuBackend>(std::move(backends));
}

std::shared_ptr<domain::IGpuBackend> criarBackendCompositoDoSistema() {
    return criarBackendCompositoCom(DepsFactoryGpuBackend{
        .criar_adapter_nvml = []() { return criarAdapterNvmlReal(); },
        .sistema_tem_intel  = &IntelI915GpuBackend::sistemaTemGpuIntel,
        .drm_root           = QStringLiteral("/sys/class/drm"),
        .proc_root          = QStringLiteral("/proc"),
    });
}

}
