#include "qprocess_shell_executor.h"
#include <QProcess>

namespace my_comp::infrastructure::collectors {

int QProcessShellExecutor::execute(const QString& command, const QStringList& args, QString& output) {
    QProcess p;
    p.start(command, args);
    if (!p.waitForFinished()) {
        return -1;
    }
    output = QString::fromUtf8(p.readAllStandardOutput());
    return p.exitCode();
}

} // namespace my_comp::infrastructure::collectors
