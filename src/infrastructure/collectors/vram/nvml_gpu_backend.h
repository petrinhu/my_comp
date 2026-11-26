#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"
#include "infrastructure/collectors/vram/nvml_adapter.h"

#include <QString>

#include <memory>

namespace my_comp::infrastructure::vram {

class NvmlGpuBackend final : public domain::vram::IGpuBackend {
public:
    explicit NvmlGpuBackend(std::shared_ptr<INvmlAdapter> adapter,
                            QString proc_root = QStringLiteral("/proc"));

    [[nodiscard]] std::vector<domain::vram::GpuInfo> enumerar_gpus() const override;

    [[nodiscard]] std::vector<domain::vram::Processo>
        enumerar_processos_vram(int gpu_id) const override;

private:
    std::shared_ptr<INvmlAdapter> m_adapter;
    QString                       m_proc_root;
};

}
