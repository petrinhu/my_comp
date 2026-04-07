#pragma once

#include "../../domain/interfaces/i_interactive_runner.h"
#include <QObject>
#include <QProcess>

namespace my_comp::infrastructure::collectors {

class InteractiveProcessRunner : public QObject, public domain::interfaces::IInteractiveRunner {
    Q_OBJECT
public:
    InteractiveProcessRunner();
    void start(const QString& cmd, const QStringList& args) override;
    void sendResponse(const QString& response) override;

signals:
    void outputReceived(const QString& output);
    void interactionRequired(const QString& question);
    void finished(int exitCode);

private:
    QProcess m_process;
};

} // namespace my_comp::infrastructure::collectors
