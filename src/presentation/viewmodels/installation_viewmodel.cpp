#include "installation_viewmodel.h"

namespace my_comp::presentation::viewmodels {

InstallationViewModel::InstallationViewModel(std::shared_ptr<infrastructure::collectors::IShellExecutor> executor, QObject* parent) 
    : QObject(parent), m_executor(executor) {
    
    auto executor_qobj = dynamic_cast<QObject*>(m_executor.get());
    if (executor_qobj) {
        connect(executor_qobj, SIGNAL(outputReceived(QString)), this, SLOT(onOutputReceived(QString)));
        connect(executor_qobj, SIGNAL(interactionRequired(QString)), this, SLOT(onInteractionRequired(QString)));
    }
}

void InstallationViewModel::startInstallation(const QString& package) {
    m_log = "Iniciando instalação de " + package + "...\n";
    emit logOutputChanged();
    m_executor->startInteractive("sudo", {"dnf", "install", "-y", package});
}

void InstallationViewModel::respond(bool confirmed) {
    m_executor->sendInput(confirmed ? "y" : "n");
    m_isWaiting = false;
    emit interactionRequired();
}

} // namespace my_comp::presentation::viewmodels
