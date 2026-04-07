#include "interactive_process_runner.h"
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

InteractiveProcessRunner::InteractiveProcessRunner() {
    connect(&m_process, &QProcess::readyReadStandardOutput, [this]() {
        QByteArray data = m_process.readAllStandardOutput();
        QString output = QString::fromUtf8(data);
        emit outputReceived(output);
        
        // Detecta padrão de confirmação (y/n)
        if (output.contains(QRegularExpression("\\[y/n\\]", QRegularExpression::CaseInsensitiveOption))) {
            emit interactionRequired(output);
        }
    });

    connect(&m_process, &QProcess::finished, this, [this](int exitCode) {
        emit finished(exitCode);
    });
}

void InteractiveProcessRunner::start(const QString& cmd, const QStringList& args) {
    m_process.start(cmd, args);
}

void InteractiveProcessRunner::sendResponse(const QString& response) {
    m_process.write(response.toUtf8());
    m_process.write("\n");
}

} // namespace my_comp::infrastructure::collectors
