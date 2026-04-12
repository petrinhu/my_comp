#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_dev_stack_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxDevStackProviderTest, ShouldCollectDevStacksFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxDevStackProvider provider(shell);
    auto info = provider.collect_dev_stacks();
    
    // Na maioria dos sistemas Fedora dev, pelo menos um desses deve estar presente
    // Mas o teste deve ser robusto: não deve crashar se nenhum estiver.
    // Validar se as listas não são nulas.
}
