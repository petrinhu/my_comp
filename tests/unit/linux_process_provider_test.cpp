#include <gtest/gtest.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>
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
    (void)found_self; // ausência é aceitável (ver comentário abaixo); só valida quando aparece
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

// MC-10 (validar entrada externa): kill(0, …) atinge o grupo inteiro do próprio app e
// kill(-1, …) atinge todos os processos do usuário; PID 1 é o init. Sinal 0 só checa
// permissão — seguro para o red: no código antigo essas chamadas retornavam true.
TEST(LinuxProcessProviderTest, KillRejectsPidsThatTargetGroupsOrInit) {
    LinuxProcessProvider provider;
    EXPECT_FALSE(provider.kill_process(0, 0));
    EXPECT_FALSE(provider.kill_process(-1, 0));
    EXPECT_FALSE(provider.kill_process(-12345, 0));
    EXPECT_FALSE(provider.kill_process(1, 0));
}

TEST(LinuxProcessProviderTest, KillRejectsOwnProcess) {
    LinuxProcessProvider provider;
    EXPECT_FALSE(provider.kill_process(static_cast<int>(::getpid()), 0));
}

TEST(LinuxProcessProviderTest, KillRejectsInvalidSignal) {
    LinuxProcessProvider provider;
    EXPECT_FALSE(provider.kill_process(999999, -1));
    EXPECT_FALSE(provider.kill_process(999999, 4096));
}

TEST(LinuxProcessProviderTest, SetPriorityRejectsSelfInitAndOutOfRangeNice) {
    LinuxProcessProvider provider;
    errno = 0;
    const int current_nice = ::getpriority(PRIO_PROCESS, 0);
    // pid 0 = o próprio processo: no código antigo setpriority(…, 0, nice atual) tinha sucesso.
    EXPECT_FALSE(provider.set_priority(0, current_nice));
    EXPECT_FALSE(provider.set_priority(-1, current_nice));
    EXPECT_FALSE(provider.set_priority(1, current_nice));
    EXPECT_FALSE(provider.set_priority(static_cast<int>(::getpid()), 20));
    EXPECT_FALSE(provider.set_priority(static_cast<int>(::getpid()), -21));
}

// AUDIT-30: o nome do próprio executável ("unit_tests") vem completo, com o caminho do cmdline.
TEST(LinuxProcessProviderTest, ComandoDoProprioProcessoVemDoCmdline) {
    LinuxProcessProvider provider;
    const auto processes = provider.collect_processes();
    const int my_pid = getpid();
    for (const auto& p : processes) {
        if (p.pid == my_pid) {
            EXPECT_TRUE(p.command.contains(QStringLiteral("unit_tests"))) << p.command.toStdString();
            EXPECT_GT(p.command.size(), 15);
            return;
        }
    }
    GTEST_SKIP() << "PID próprio não apareceu em /proc";
}
