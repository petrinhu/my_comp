#ifndef I_SHELL_EXECUTOR_H
#define I_SHELL_EXECUTOR_H

#include <QString>
#include <QStringList>

namespace my_comp::infrastructure::collectors {

class IShellExecutor {
public:
    virtual ~IShellExecutor() = default;
    virtual int execute(const QString& command, const QStringList& args, QString& output) = 0;
};

} // namespace my_comp::infrastructure::collectors

#endif // I_SHELL_EXECUTOR_H
