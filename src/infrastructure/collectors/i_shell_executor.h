#ifndef I_SHELL_EXECUTOR_H
#define I_SHELL_EXECUTOR_H

#include <QString>
#include <QStringList>

namespace my_comp::infrastructure::collectors {

// Timeouts por natureza de operação. Consultas de gerenciador de pacote vão à rede
// (refresh de metadados) e o benchmark do hdparm leva ~6–10 s por design.
inline constexpr int DEFAULT_SHELL_TIMEOUT_MS = 5'000;
inline constexpr int PACKAGE_MANAGER_TIMEOUT_MS = 180'000;
inline constexpr int BENCHMARK_TIMEOUT_MS = 60'000;

// Saída completa de um comando: stdout, stderr e código de saída (-1 = não rodou/timeout).
struct ShellResult {
    int exit_code = -1;
    QString out;
    QString err;
};

class IShellExecutor {
public:
    virtual ~IShellExecutor() = default;
    virtual int execute(const QString& command, const QStringList& args, QString& output) = 0;

    // Implementação padrão ignora o timeout (dublês de teste só precisam de execute()).
    virtual int execute_with_timeout(const QString& command, const QStringList& args,
                                     QString& output, int timeout_ms) {
        (void)timeout_ms;
        return execute(command, args, output);
    }

    // Como execute_with_timeout, mas devolve também o stderr (para mostrar ao usuário).
    // Implementação padrão (dublês de teste): sem stderr.
    virtual ShellResult run_capturing(const QString& command, const QStringList& args,
                                      int timeout_ms = DEFAULT_SHELL_TIMEOUT_MS) {
        ShellResult r;
        r.exit_code = execute_with_timeout(command, args, r.out, timeout_ms);
        return r;
    }
};

} // namespace my_comp::infrastructure::collectors

#endif // I_SHELL_EXECUTOR_H
