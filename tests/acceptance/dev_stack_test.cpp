// QA-03 — Aceitação: aba Dev Tools pela pilha real (DevStackViewModel → LinuxDevStackProvider).
#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>

#include "scripted_shell.h"
#include "../../src/infrastructure/collectors/linux_dev_stack_provider.h"
#include "../../src/presentation/viewmodels/dev_stack_viewmodel.h"

using namespace my_comp::infrastructure::collectors;
using my_comp::presentation::viewmodels::DevStackViewModel;
using my_comp::tests::ScriptedShell;

TEST(DevStackAcceptance, UserSeesInstalledToolchainsAndContainers) {
    auto shell = std::make_shared<ScriptedShell>();
    shell->reply("gcc --version", "gcc (GCC) 15.2.1 20251022 (Red Hat 15.2.1-3)\nCopyright (C) 2025\n");
    shell->reply("java -version", {}, 0, "openjdk version \"21.0.9\" 2025-10-21\nOpenJDK Runtime Environment");
    shell->reply("podman ps -a --format json",
                 R"([{"Id":"3f2a","Names":["web"],"Image":"docker.io/library/nginx:latest","State":"running"}])");
    // rustc não roteirizado = não instalado; não deve aparecer.
    const std::vector<ToolchainSpec> catalog = {
        {"GCC", "gcc", {"--version"}}, {"Java", "java", {"-version"}}, {"Rust", "rustc", {"--version"}}};

    DevStackViewModel vm(std::make_shared<LinuxDevStackProvider>(shell, catalog));
    QSignalSpy changed(&vm, &DevStackViewModel::dataChanged);
    vm.refresh();
    ASSERT_TRUE(changed.wait(5000));

    QStringList tools;
    for (const auto& t : vm.toolchains()) tools << t.toMap()["name"].toString() + " " + t.toMap()["version"].toString();
    EXPECT_TRUE(tools.contains("GCC 15.2.1")) << tools.join(", ").toStdString();
    EXPECT_TRUE(tools.contains("Java 21.0.9")) << tools.join(", ").toStdString();
    EXPECT_EQ(tools.size(), 2);

    ASSERT_EQ(vm.containers().size(), 1);
    const auto c = vm.containers().at(0).toMap();
    EXPECT_EQ(c["name"].toString(), QString("web"));
    EXPECT_EQ(c["runtime"].toString(), QString("Podman"));
}
