#pragma once

#include "../../domain/interfaces/i_interactive_runner.h"
#include <QObject>
#include <QProcess>
#include <functional>

namespace my_comp::infrastructure::collectors {

class InteractiveProcessRunner : public QObject, public domain::interfaces::IInteractiveRunner {
    Q_OBJECT
public:
    InteractiveProcessRunner();
    void start(const QString& cmd, const QStringList& args) override;
    void sendResponse(const QString& response) override;

    void set_output_handler(std::function<void(const QString&)> handler) override { m_on_output = std::move(handler); }
    void set_interaction_handler(std::function<void(const QString&)> handler) override { m_on_interaction = std::move(handler); }
    void set_finished_handler(std::function<void(int)> handler) override { m_on_finished = std::move(handler); }

private:
    QProcess m_process;
    std::function<void(const QString&)> m_on_output;
    std::function<void(const QString&)> m_on_interaction;
    std::function<void(int)> m_on_finished;
};

} // namespace my_comp::infrastructure::collectors
