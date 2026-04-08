#include "installer_adapter.h"
#include "interactive_process_runner.h"

namespace my_comp::infrastructure::collectors {

InstallerAdapter::InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner)
    : m_runner(std::move(runner)) {
    
    auto interactive_runner = dynamic_cast<InteractiveProcessRunner*>(m_runner.get());
    if (interactive_runner) {
        connect(interactive_runner, &InteractiveProcessRunner::outputReceived, this, [this](const QString& output){
            if (onOutput) onOutput(output);
        });
        connect(interactive_runner, &InteractiveProcessRunner::interactionRequired, this, [this](const QString& question){
            if (onInteraction) onInteraction(question);
        });
        connect(interactive_runner, &InteractiveProcessRunner::finished, this, [this](int exitCode){
            if (onFinished) onFinished(exitCode);
        });
    }
}

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
        case domain::models::ProviderType::Npm:
            cmd = "sudo";
            args = {"npm", "install", "-g", package.name};
            break;
    }
    m_runner->start(cmd, args);
}

void InstallerAdapter::upgradeAll() {
    QString cmd = "sudo";
    QStringList args = {"dnf", "upgrade", "-y"};
    m_runner->start(cmd, args);
}

void InstallerAdapter::sendResponse(const QString& response) {
    m_runner->sendResponse(response);
}

} // namespace my_comp::infrastructure::collectors
