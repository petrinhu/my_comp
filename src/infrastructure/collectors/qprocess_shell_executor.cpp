#include "qprocess_shell_executor.h"
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

namespace my_comp::infrastructure::collectors {

int QProcessShellExecutor::execute(const QString& command, const QStringList& args, QString& output) {
    qDebug() << "[Shell] Executing:" << command << args.join(' ');
    QProcess p;
    // IN-01b: ambiente explícito com locale C — parsing determinístico em qualquer sistema.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");
    env.insert("LANG", "C");
    p.setProcessEnvironment(env);
    p.start(command, args);
    if (!p.waitForFinished(5000)) {
        qWarning() << "[Shell] Timeout executing:" << command;
        p.kill();
        return -1;
    }
    output = QString::fromUtf8(p.readAllStandardOutput());
    // IN-01a: captura stderr explicitamente; loga em qWarning se não-vazio (antes era silencioso).
    const QString errOutput = QString::fromUtf8(p.readAllStandardError()).trimmed();
    int exitCode = p.exitCode();
    if (!errOutput.isEmpty()) {
        qWarning() << "[Shell] stderr from" << command << ":" << errOutput;
    }
    qDebug() << "[Shell] Command" << command << "finished with exit code" << exitCode;
    return exitCode;
}

} // namespace my_comp::infrastructure::collectors
