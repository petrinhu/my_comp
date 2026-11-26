#include "qprocess_shell_executor.h"
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

namespace my_comp::infrastructure::collectors {

int QProcessShellExecutor::execute(const QString& command, const QStringList& args, QString& output) {
    return execute_with_timeout(command, args, output, DEFAULT_SHELL_TIMEOUT_MS);
}

int QProcessShellExecutor::execute_with_timeout(const QString& command, const QStringList& args,
                                                QString& output, int timeout_ms) {
    qDebug() << "[Shell] Executing:" << command << args.join(' ');
    QProcess p;
    // IN-01b: ambiente explícito com locale C — parsing determinístico em qualquer sistema.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");
    env.insert("LANG", "C");
    p.setProcessEnvironment(env);
    p.start(command, args);
    if (!p.waitForStarted(timeout_ms)) {
        qWarning() << "[Shell] Failed to start:" << command << p.errorString();
        return -1;
    }
    if (!p.waitForFinished(timeout_ms)) {
        qWarning() << "[Shell] Timeout (" << timeout_ms << "ms) executing:" << command;
        p.kill();
        p.waitForFinished(1000); // colhe o processo morto (evita zumbi)
        return -1;
    }
    if (p.exitStatus() == QProcess::CrashExit) {
        qWarning() << "[Shell] Crashed:" << command;
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
