#include "system_info_viewmodel.h"

namespace my_comp::presentation::viewmodels {

SystemInfoViewModel::SystemInfoViewModel(std::shared_ptr<domain::interfaces::ISystemInfoProvider> provider, QObject* parent)
    : QObject(parent) {
    auto info = provider->get_system_info();
    m_distro = info.distroName + " " + info.distroVersion;
    m_kernel = info.kernelVersion;
    m_driver = info.gpuDriver;
}

} // namespace my_comp::presentation::viewmodels
