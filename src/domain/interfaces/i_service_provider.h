#pragma once
#include "../models/service_info.h"
#include <vector>

namespace my_comp::domain::interfaces {

class IServiceProvider {
public:
    virtual ~IServiceProvider() = default;
    virtual std::vector<models::ServiceInfo> get_services() = 0;
    virtual bool start_service(const QString& name) = 0;   // enable --now
    virtual bool pause_service(const QString& name) = 0;   // stop
    virtual bool stop_service(const QString& name) = 0;    // disable --now
    virtual bool create_daemon(const QString& exe_path, const QString& params) = 0;
};

} // namespace my_comp::domain::interfaces
