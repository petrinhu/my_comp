#pragma once

#include "../../domain/interfaces/i_installer.h"
#include "../../domain/interfaces/i_interactive_runner.h"
#include <QObject>
#include <functional>
#include <memory>

namespace my_comp::infrastructure::collectors {

// Nome de pacote vindo do parsing de dnf/apt/pacman/etc. é entrada externa (MC-10).
// Aceita só o alfabeto dos gerenciadores suportados e nunca começa por '-'
// (impede injeção de opção no comando que roda como root).
[[nodiscard]] bool is_safe_package_name(const QString& name);

class InstallerAdapter : public QObject, public domain::interfaces::IInstaller {
    Q_OBJECT
public:
    explicit InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner);
    void install(const domain::models::UpdatePackage& package) override;
    void upgradeAll() override;
    void sendResponse(const QString& response) override;

    void set_output_handler(std::function<void(const QString&)> handler) override;
    void set_interaction_handler(std::function<void(const QString&)> handler) override;
    void set_finished_handler(std::function<void(int)> handler) override;

private:
    void reject(const QString& reason);

    std::shared_ptr<domain::interfaces::IInteractiveRunner> m_runner;
    std::function<void(const QString&)> m_on_output;
    std::function<void(int)> m_on_finished;
};

} // namespace my_comp::infrastructure::collectors
