#include "interactive_process_runner.h"
#include <QRegularExpression>

namespace my_comp::infrastructure::collectors {

InteractiveProcessRunner::InteractiveProcessRunner() {
    connect(&m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        const QByteArray data = m_process.readAllStandardOutput();
        const QString output = QString::fromUtf8(data);
        if (m_on_output) m_on_output(output);

        if (output.contains(QRegularExpression("\\[y/n\\]", QRegularExpression::CaseInsensitiveOption))) {
            if (m_on_interaction) m_on_interaction(output);
        }
    });

    connect(&m_process, &QProcess::finished, this, [this](int exitCode) {
        if (m_on_finished) m_on_finished(exitCode);
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
