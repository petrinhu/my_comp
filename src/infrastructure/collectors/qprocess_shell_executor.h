#ifndef QPROCESS_SHELL_EXECUTOR_H
#define QPROCESS_SHELL_EXECUTOR_H

#include <QProcess>
#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

class QProcessShellExecutor : public IShellExecutor {
public:
    int execute(const QString& command, const QStringList& args, QString& output) override {
        QProcess process;
        process.start(command, args);
        if (!process.waitForFinished()) {
            return -1;
        }
        output = QString::fromUtf8(process.readAllStandardOutput());
        return process.exitCode();
    }
};

} // namespace my_comp::infrastructure::collectors

#endif // QPROCESS_SHELL_EXECUTOR_H
