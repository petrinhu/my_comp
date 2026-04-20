#pragma once
#include "../../domain/interfaces/i_gpu_provider.h"
#include <QString>

namespace my_comp::infrastructure::collectors {

class SysfsGpuProvider : public domain::interfaces::IGpuProvider {
public:
    SysfsGpuProvider() : m_drm_root("/sys/class/drm") {}
    explicit SysfsGpuProvider(QString drm_root) : m_drm_root(std::move(drm_root)) {}

    std::vector<domain::models::GpuStats> collect_stats() override;

private:
    QString m_drm_root;
};

} // namespace my_comp::infrastructure::collectors
