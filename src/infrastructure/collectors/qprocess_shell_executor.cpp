#include "qprocess_shell_executor.h"
#include <QProcess>
#include <QDebug>

namespace my_comp::infrastructure::collectors {

int QProcessShellExecutor::execute(const QString& command, const QStringList& args, QString& output) {
    qDebug() << "[Shell] Executing:" << command << args.join(' ');
    QProcess p;
    p.start(command, args);
    if (!p.waitForFinished(5000)) { // Timeout de 5s
        qWarning() << "[Shell] Timeout executing:" << command;
        p.kill();
        return -1;
    }
    output = QString::fromUtf8(p.readAllStandardOutput());
    int exitCode = p.exitCode();
    qDebug() << "[Shell] Command" << command << "finished with exit code" << exitCode;
    return exitCode;
}

} // namespace my_comp::infrastructure::collectors
