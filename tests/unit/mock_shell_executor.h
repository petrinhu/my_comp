#pragma once
#include <gmock/gmock.h>
#include "../src/infrastructure/collectors/i_shell_executor.h"

namespace my_comp::tests {

class MockShellExecutor : public infrastructure::collectors::IShellExecutor {
public:
    MOCK_METHOD(int, execute, (const QString&, const QStringList&, QString&), (override));
    MOCK_METHOD(void, startInteractive, (const QString&, const QStringList&), (override));
    MOCK_METHOD(void, sendInput, (const QString&), (override));
    
    MockShellExecutor() = default;
};

} // namespace my_comp::tests
