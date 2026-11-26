// QA-03 — Aceitação: explorador de processos pela pilha real (ProcessViewModel →
// LinuxProcessProvider) sobre o /proc deste processo.
#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>
#include <unistd.h>

#include "../../src/infrastructure/collectors/linux_process_provider.h"
#include "../../src/presentation/viewmodels/process_viewmodel.h"

using my_comp::infrastructure::collectors::LinuxProcessProvider;
using my_comp::presentation::viewmodels::ProcessViewModel;

TEST(ProcessMonitorAcceptance, UserFindsOwnProcessByFilter) {
    ProcessViewModel vm(std::make_shared<LinuxProcessProvider>());
    QSignalSpy changed(&vm, &ProcessViewModel::processesChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(5000));
    ASSERT_FALSE(vm.processes().isEmpty());

    // A coluna mostra o comm do /proc/<pid>/stat, que o kernel corta em 15 caracteres
    // (TASK_COMM_LEN): "acceptance_tests" aparece como "acceptance_test".
    const QString comm = QStringLiteral("acceptance_tests").left(15);
    vm.setFilter(comm);
    bool found_self = false;
    for (const auto& p : vm.processes()) {
        const auto m = p.toMap();
        EXPECT_TRUE(m["command"].toString().contains(comm) || m["user"].toString().contains(comm));
        found_self = found_self || m["pid"].toInt() == static_cast<int>(::getpid());
    }
    EXPECT_TRUE(found_self);
}

// MC-10: pedir para matar o próprio app ou o grupo pela UI é recusado. Ação aceita faz o
// VM recarregar a lista (processesChanged); recusada, não há refresh.
TEST(ProcessMonitorAcceptance, KillingTheAppItselfIsRefused) {
    ProcessViewModel vm(std::make_shared<LinuxProcessProvider>());
    QSignalSpy changed(&vm, &ProcessViewModel::processesChanged);
    vm.killProcess(static_cast<int>(::getpid()), 0);
    vm.killProcess(0, 0);
    vm.killProcess(-1, 0);
    EXPECT_FALSE(changed.wait(800));
}
