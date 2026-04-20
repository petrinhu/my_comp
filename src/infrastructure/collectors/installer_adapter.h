#pragma once

#include "../../domain/interfaces/i_installer.h"
#include "../../domain/interfaces/i_interactive_runner.h"
#include <QObject>
#include <functional>
#include <memory>

namespace my_comp::infrastructure::collectors {

class InstallerAdapter : public QObject, public domain::interfaces::IInstaller {
    Q_OBJECT
public:
    explicit InstallerAdapter(std::shared_ptr<domain::interfaces::IInteractiveRunner> runner);
    void install(const domain::models::UpdatePackage& package) override;
    void upgradeAll() override;
    void sendResponse(const QString& response) override;

    void set_output_handler(std::function<void(const QString&)> handler) override { m_runner->set_output_handler(std::move(handler)); }
    void set_interaction_handler(std::function<void(const QString&)> handler) override { m_runner->set_interaction_handler(std::move(handler)); }
    void set_finished_handler(std::function<void(int)> handler) override { m_runner->set_finished_handler(std::move(handler)); }

private:
    std::shared_ptr<domain::interfaces::IInteractiveRunner> m_runner;
};

} // namespace my_comp::infrastructure::collectors
