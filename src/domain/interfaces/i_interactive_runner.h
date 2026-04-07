#pragma once

#include <QString>
#include <QStringList>

namespace my_comp::domain::interfaces {

class IInteractiveRunner {
public:
    virtual ~IInteractiveRunner() = default;

    virtual void start(const QString& cmd, const QStringList& args) = 0;
    virtual void sendResponse(const QString& response) = 0;
};

} // namespace my_comp::domain::interfaces
