#pragma once
// Fronteira com o SO controlada para os cenários de aceitação (QA-03): responde por
// "comando args..." com stdout/stderr/exit de saídas reais; comando não roteirizado = ausente.
#include "../../src/infrastructure/collectors/i_shell_executor.h"

#include <QStringList>
#include <map>
#include <mutex>

namespace my_comp::tests {

class ScriptedShell : public infrastructure::collectors::IShellExecutor {
public:
    void reply(const QString& command_line, const QString& out, int exit_code = 0, const QString& err = {}) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_replies[command_line] = {.exit_code = exit_code, .out = out, .err = err};
    }
    [[nodiscard]] QStringList calls() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_calls;
    }

    int execute(const QString& command, const QStringList& args, QString& output) override {
        const auto r = run_capturing(command, args, infrastructure::collectors::DEFAULT_SHELL_TIMEOUT_MS);
        output = r.out;
        return r.exit_code;
    }
    infrastructure::collectors::ShellResult run_capturing(const QString& command, const QStringList& args, int) override {
        const QString key = (QStringList{command} + args).join(' ');
        std::lock_guard<std::mutex> lock(m_mutex); // providers chamam de várias threads (QtConcurrent)
        m_calls << key;
        auto it = m_replies.find(key);
        if (it == m_replies.end()) return {.exit_code = -1, .out = {}, .err = "não instalado"};
        return it->second;
    }

private:
    mutable std::mutex m_mutex;
    std::map<QString, infrastructure::collectors::ShellResult> m_replies;
    QStringList m_calls;
};

} // namespace my_comp::tests
