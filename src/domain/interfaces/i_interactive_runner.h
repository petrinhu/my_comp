#pragma once

#include <QString>
#include <QStringList>
#include <functional>

namespace my_comp::domain::interfaces {

class IInteractiveRunner {
public:
    virtual ~IInteractiveRunner() = default;

    virtual void start(const QString& cmd, const QStringList& args) = 0;
    virtual void sendResponse(const QString& response) = 0;

    virtual void set_output_handler(std::function<void(const QString&)> handler) = 0;
    virtual void set_interaction_handler(std::function<void(const QString&)> handler) = 0;
    virtual void set_finished_handler(std::function<void(int)> handler) = 0;
};

} // namespace my_comp::domain::interfaces
