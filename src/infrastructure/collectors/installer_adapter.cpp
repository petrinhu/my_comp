#include "installer_adapter.h"

namespace my_comp::infrastructure::collectors {

InstallerAdapter::InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner)
    : m_runner(std::move(runner)) {}

void InstallerAdapter::install(const domain::models::UpdatePackage& package) {
    QString cmd;
    QStringList args;

    switch (package.provider) {
        case domain::models::ProviderType::Dnf:
            cmd = "sudo";
            args = {"dnf", "install", "-y", package.name};
            break;
        case domain::models::ProviderType::Flatpak:
            cmd = "flatpak";
            args = {"install", "-y", package.name};
            break;
        case domain::models::ProviderType::Snap:
            cmd = "snap";
            args = {"install", package.name};
            break;
        default:
            return;
    }
    m_runner->start(cmd, args);
}

} // namespace my_comp::infrastructure::collectors
