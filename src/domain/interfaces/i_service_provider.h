#pragma once
#include "../models/operation_result.h"
#include "../models/service_info.h"
#include <vector>

namespace my_comp::domain::interfaces {

class IServiceProvider {
public:
    virtual ~IServiceProvider() = default;
    virtual std::vector<models::ServiceInfo> get_services() = 0;
    // SV-02b: nomes dizem se a ação é só de runtime ou também persiste no boot.
    virtual models::OperationResult enable_and_start(const QString& name, models::ServiceScope scope) = 0;  // enable --now
    virtual models::OperationResult stop(const QString& name, models::ServiceScope scope) = 0;              // stop (só esta sessão)
    virtual models::OperationResult disable_and_stop(const QString& name, models::ServiceScope scope) = 0;  // disable --now
    // Cria e liga um unit systemd --user para exe_path; message explica a falha ou nomeia o unit.
    virtual models::OperationResult create_daemon(const QString& exe_path, const QString& params) = 0;
};

} // namespace my_comp::domain::interfaces
