#pragma once
#include <QString>
#include <functional>
#include "../models/update_package.h"

namespace my_comp::domain::interfaces {

class IInstaller {
public:
    virtual ~IInstaller() = default;
    virtual void install(const models::UpdatePackage& package) = 0;
    virtual void upgradeAll() = 0;
    virtual void sendResponse(const QString& response) = 0;

    virtual void set_output_handler(std::function<void(const QString&)> handler) = 0;
    virtual void set_interaction_handler(std::function<void(const QString&)> handler) = 0;
    virtual void set_finished_handler(std::function<void(int)> handler) = 0;
};

} // namespace my_comp::domain::interfaces
