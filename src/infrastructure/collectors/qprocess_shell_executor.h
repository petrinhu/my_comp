#ifndef QPROCESS_SHELL_EXECUTOR_H
#define QPROCESS_SHELL_EXECUTOR_H

#include "i_shell_executor.h"

namespace my_comp::infrastructure::collectors {

class QProcessShellExecutor : public IShellExecutor {
public:
    int execute(const QString& command, const QStringList& args, QString& output) override;
    int execute_with_timeout(const QString& command, const QStringList& args, QString& output,
                             int timeout_ms) override;
};

} // namespace my_comp::infrastructure::collectors

#endif // QPROCESS_SHELL_EXECUTOR_H
