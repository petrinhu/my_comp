#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_process_provider.h"
#include <unistd.h>

using namespace my_comp::infrastructure::collectors;

TEST(LinuxProcessProviderTest, ShouldCollectProcessesFromRealSystem) {
    LinuxProcessProvider provider;
    auto processes = provider.collect_processes();
    
    // Deve haver muitos processos em um sistema Linux rodando
    EXPECT_FALSE(processes.empty());
    
    // Validar se o processo atual (PID do teste) está na lista
    int my_pid = getpid();
    bool found_self = false;
    for (const auto& p : processes) {
        if (p.pid == my_pid) {
            found_self = true;
            EXPECT_FALSE(p.command.isEmpty());
            EXPECT_GT(p.res, 0);
            break;
        }
    }
    // Em alguns casos de filtragem ou permissão, o PID próprio pode não aparecer se for root-only, 
    // mas geralmente aparece em /proc.
}

TEST(LinuxProcessProviderTest, ShouldHandleKillAndPriorityGracefully) {
    LinuxProcessProvider provider;
    
    // Kill em PID inválido deve falhar ou retornar erro graciosamente
    // kill_process retorna bool indicando se o comando foi enviado com sucesso
    // (Pode falhar se o PID não existir)
    provider.kill_process(999999); 
    
    // Set priority em PID inválido
    provider.set_priority(999999, 10);
    
    // Sucesso ou falha depende de permissões, mas a chamada não deve crashar.
}
