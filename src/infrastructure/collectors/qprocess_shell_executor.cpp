#include "qprocess_shell_executor.h"
#include "../shutdown_signal.h"
#include <QElapsedTimer>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

#include <algorithm>

namespace my_comp::infrastructure::collectors {

int QProcessShellExecutor::execute(const QString& command, const QStringList& args, QString& output) {
    return execute_with_timeout(command, args, output, DEFAULT_SHELL_TIMEOUT_MS);
}

int QProcessShellExecutor::execute_with_timeout(const QString& command, const QStringList& args,
                                                QString& output, int timeout_ms) {
    auto r = run_capturing(command, args, timeout_ms);
    output = std::move(r.out);
    return r.exit_code;
}

ShellResult QProcessShellExecutor::run_capturing(const QString& command, const QStringList& args,
                                                 int timeout_ms) {
    ShellResult result;
    if (shutdown_requested()) {
        result.err = QStringLiteral("app encerrando");
        return result;
    }
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
        result.err = p.errorString();
        return result;
    }
    // Espera em fatias para atender o encerramento do app sem esperar o timeout inteiro
    // (dnf check-update chega a 180 s). Revisão do UX-05.
    constexpr int FATIA_MS = 200;
    QElapsedTimer relogio;
    relogio.start();
    bool terminou = false;
    while (!terminou) {
        const qint64 restante = timeout_ms - relogio.elapsed();
        if (restante <= 0 || shutdown_requested()) {
            break;
        }
        terminou = p.waitForFinished(static_cast<int>(std::min<qint64>(restante, FATIA_MS)));
        if (!terminou && p.state() == QProcess::NotRunning) {
            terminou = true;  // acabou entre duas fatias
        }
    }
    if (!terminou) {
        const bool por_encerramento = shutdown_requested();
        qWarning() << "[Shell]" << (por_encerramento ? "Encerramento do app interrompeu:" : "Timeout executando:")
                   << command;
        p.kill();
        p.waitForFinished(1000); // colhe o processo morto (evita zumbi)
        result.err = por_encerramento ? QStringLiteral("app encerrando")
                                      : QStringLiteral("tempo esgotado (%1 ms)").arg(timeout_ms);
        return result;
    }
    if (p.exitStatus() == QProcess::CrashExit) {
        qWarning() << "[Shell] Crashed:" << command;
        result.err = QStringLiteral("processo terminou de forma anormal");
        return result;
    }
    result.out = QString::fromUtf8(p.readAllStandardOutput());
    // IN-01a: stderr capturado e devolvido (SV-02c); também logado se não-vazio.
    result.err = QString::fromUtf8(p.readAllStandardError()).trimmed();
    result.exit_code = p.exitCode();
    if (!result.err.isEmpty()) {
        qWarning() << "[Shell] stderr from" << command << ":" << result.err;
    }
    qDebug() << "[Shell] Command" << command << "finished with exit code" << result.exit_code;
    return result;
}

} // namespace my_comp::infrastructure::collectors
