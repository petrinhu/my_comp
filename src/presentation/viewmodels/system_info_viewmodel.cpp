#include "system_info_viewmodel.h"

namespace my_comp::presentation::viewmodels {

SystemInfoViewModel::SystemInfoViewModel(std::shared_ptr<domain::interfaces::ISystemInfoProvider> provider, QObject* parent)
    : QObject(parent), m_provider(provider) {
}

void SystemInfoViewModel::refresh() {
    if (!m_provider) return;
    auto info = m_provider->get_system_info();
    m_distro = info.distroName + " " + info.distroVersion;
    m_kernel = info.kernelVersion;
    m_driver = info.gpuDriver;
    emit dataChanged(); 
}

} // namespace my_comp::presentation::viewmodels
