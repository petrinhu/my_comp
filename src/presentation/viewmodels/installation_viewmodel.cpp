#include "installation_viewmodel.h"

namespace my_comp::presentation::viewmodels {

InstallationViewModel::InstallationViewModel(std::shared_ptr<domain::interfaces::IInstaller> installer, QObject* parent) 
    : QObject(parent), m_installer(installer) {
    
    // Configura callbacks da interface de domínio (Foundation pura)
    m_installer->onOutput = [this](const QString& output) {
        onOutputReceived(output);
    };
    
    m_installer->onInteraction = [this](const QString& question) {
        onInteractionRequired(question);
    };

    m_installer->onFinished = [this](int exitCode) {
        m_log += "\n--- Processo finalizado com código: " + QString::number(exitCode) + " ---\n";
        emit logOutputChanged();
    };
}

void InstallationViewModel::startInstallation(const QString& name, int providerType) {
    domain::models::UpdatePackage pkg;
    pkg.name = name;
    pkg.provider = static_cast<domain::models::ProviderType>(providerType);
    
    m_log = "Iniciando instalação de " + name + "...\n";
    emit logOutputChanged();
    m_installer->install(pkg);
}

void InstallationViewModel::startUpgradeAll() {
    m_log = "Iniciando atualização completa do sistema (dnf upgrade)...\n";
    emit logOutputChanged();
    m_installer->upgradeAll();
}

void InstallationViewModel::respond(bool confirmed) {
    m_log += (confirmed ? "Respondendo: Sim (y)\n" : "Respondendo: Não (n)\n");
    emit logOutputChanged();
    m_installer->sendResponse(confirmed ? "y" : "n");
    m_isWaiting = false;
    emit interactionRequired();
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
