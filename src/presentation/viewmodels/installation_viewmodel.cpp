#include "installation_viewmodel.h"
#include "../../infrastructure/collectors/installer_adapter.h"

namespace my_comp::presentation::viewmodels {

InstallationViewModel::InstallationViewModel(std::shared_ptr<domain::interfaces::IInstaller> installer, QObject* parent) 
    : QObject(parent), m_installer(installer) {
    
    auto runner_adapter = dynamic_cast<my_comp::infrastructure::collectors::InstallerAdapter*>(m_installer.get());
}

void InstallationViewModel::startInstallation(const QString& name, int providerType) {
    domain::models::UpdatePackage pkg;
    pkg.name = name;
    pkg.provider = static_cast<domain::models::ProviderType>(providerType);
    
    m_log = "Iniciando instalação de " + name + "...\n";
    emit logOutputChanged();
    m_installer->install(pkg);
}

void InstallationViewModel::respond(bool confirmed) {
    // Lógica de resposta será implementada via canal de comunicação do runner
}

void InstallationViewModel::onOutputReceived(const QString& output) {
    m_log += output;
    emit logOutputChanged();
}

void InstallationViewModel::onInteractionRequired(const QString& question) {
    m_question = question;
    m_isWaiting = true;
    emit interactionRequired();
}

} // namespace my_comp::presentation::viewmodels
