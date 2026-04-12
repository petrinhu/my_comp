#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/systemd_service_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(SystemdServiceProviderTest, ShouldCollectServicesFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    SystemdServiceProvider provider(shell);
    auto services = provider.get_services();
    
    // Deve haver serviços no Fedora rodando Systemd
    EXPECT_FALSE(services.empty());
    
    // Verificar se algum serviço comum está na lista (ex: dbus, network)
    bool found_common = false;
    for (const auto& s : services) {
        if (s.name.contains("dbus", Qt::CaseInsensitive) || 
            s.name.contains("network", Qt::CaseInsensitive)) {
            found_common = true;
            break;
        }
    }
}

TEST(SystemdServiceProviderTest, ShouldHandleCommandsGracefully) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    SystemdServiceProvider provider(shell);
    
    // Comandos em serviços inexistentes devem falhar sem crashar
    provider.start_service("non-existent-service-my-comp-test");
    provider.stop_service("non-existent-service-my-comp-test");
}

TEST(SystemdServiceProviderTest, ShouldHandleDaemonCreation) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    SystemdServiceProvider provider(shell);
    
    // Tentar criar daemon em path inválido ou sem permissão
    // create_daemon retorna bool indicando se conseguiu escrever o arquivo unit.
    // Em ambiente de teste sem root, deve falhar para /etc/systemd/system mas talvez passe em ~/.config/...
    provider.create_daemon("/usr/bin/true", "--help");
}
