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

    // Callbacks para desacoplar do QObject na Foundation
    std::function<void(const QString&)> onOutput;
    std::function<void(const QString&)> onInteraction;
    std::function<void(int)> onFinished;
};

} // namespace my_comp::domain::interfaces
