#pragma once
#include "../../domain/interfaces/i_service_provider.h"
#include "i_shell_executor.h"
#include <memory>

namespace my_comp::infrastructure::collectors {

class SystemdServiceProvider : public domain::interfaces::IServiceProvider {
public:
    explicit SystemdServiceProvider(std::shared_ptr<IShellExecutor> executor);
    
    std::vector<domain::models::ServiceInfo> get_services() override;
    bool start_service(const QString& name) override;
    bool pause_service(const QString& name) override;
    bool stop_service(const QString& name) override;
    bool create_daemon(const QString& exe_path, const QString& params) override;

private:
    std::shared_ptr<IShellExecutor> m_executor;
    
    std::vector<domain::models::ServiceInfo> collect_units(bool user_mode);
    bool run_systemctl(const QStringList& args, bool user_mode = false);
};

} // namespace my_comp::infrastructure::collectors
