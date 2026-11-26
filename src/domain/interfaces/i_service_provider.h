#pragma once
#include "../models/operation_result.h"
#include "../models/service_info.h"
#include <vector>

namespace my_comp::domain::interfaces {

class IServiceProvider {
public:
    virtual ~IServiceProvider() = default;
    virtual std::vector<models::ServiceInfo> get_services() = 0;
    virtual bool start_service(const QString& name, models::ServiceScope scope) = 0;   // enable --now
    virtual bool pause_service(const QString& name, models::ServiceScope scope) = 0;   // stop
    virtual bool stop_service(const QString& name, models::ServiceScope scope) = 0;    // disable --now
    // Cria e liga um unit systemd --user para exe_path; message explica a falha ou nomeia o unit.
    virtual models::OperationResult create_daemon(const QString& exe_path, const QString& params) = 0;
};

} // namespace my_comp::domain::interfaces
