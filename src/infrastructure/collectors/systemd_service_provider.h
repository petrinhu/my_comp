#pragma once
#include "../../domain/interfaces/i_service_provider.h"
#include "i_shell_executor.h"
#include <QHash>
#include <memory>

namespace my_comp::infrastructure::collectors {

class SystemdServiceProvider : public domain::interfaces::IServiceProvider {
public:
    explicit SystemdServiceProvider(std::shared_ptr<IShellExecutor> executor);
    
    std::vector<domain::models::ServiceInfo> get_services() override;
    domain::models::OperationResult enable_and_start(const QString& name, domain::models::ServiceScope scope) override;
    domain::models::OperationResult stop(const QString& name, domain::models::ServiceScope scope) override;
    domain::models::OperationResult disable_and_stop(const QString& name, domain::models::ServiceScope scope) override;
    domain::models::OperationResult create_daemon(const QString& exe_path, const QString& params) override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
    
    std::vector<domain::models::ServiceInfo> collect_units(domain::models::ServiceScope scope);
    // unit_file → state ("enabled", "disabled", "static"...) de `list-unit-files`.
    QHash<QString, QString> collect_unit_file_states(domain::models::ServiceScope scope);
    bool run_systemctl(const QStringList& args, bool user_mode = false);
    // Executa a ação e monta a mensagem: sucesso → `done`; falha → `failed` + 1ª linha do stderr.
    domain::models::OperationResult control(const QStringList& args, domain::models::ServiceScope scope,
                                            const QString& done, const QString& failed);
};

} // namespace my_comp::infrastructure::collectors
