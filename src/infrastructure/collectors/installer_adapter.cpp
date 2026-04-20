#include "installer_adapter.h"
#include <QFile>

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
            args = {"refresh", package.name};
            break;
        case domain::models::ProviderType::Cargo:
            cmd = "cargo";
            args = {"install-update", package.name};
            break;
        case domain::models::ProviderType::Apt:
            cmd = "sudo";
            args = {"apt", "install", "-y", package.name};
            break;
        case domain::models::ProviderType::Pacman:
            cmd = "sudo";
            args = {"pacman", "-S", "--noconfirm", package.name};
            break;

        case domain::models::ProviderType::Npm:
            cmd = "sudo";
            args = {"npm", "install", "-g", package.name};
            break;
    }
    m_runner->start(cmd, args);
}

void InstallerAdapter::upgradeAll() {
    QString cmd;
    QStringList args;

    if (QFile::exists("/usr/bin/dnf")) {
        cmd = "sudo";
        args = {"dnf", "upgrade", "-y"};
    } else if (QFile::exists("/usr/bin/apt")) {
        cmd = "sudo";
        args = {"apt", "upgrade", "-y"};
    } else if (QFile::exists("/usr/bin/pacman")) {
        cmd = "sudo";
        args = {"pacman", "-Syu", "--noconfirm"};
    } else {
        return;
    }

    m_runner->start(cmd, args);
}

void InstallerAdapter::sendResponse(const QString& response) {
    m_runner->sendResponse(response);
}

} // namespace my_comp::infrastructure::collectors
