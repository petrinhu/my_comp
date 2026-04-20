#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_dev_stack_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxDevStackProviderTest, ShouldCollectDevStacksFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxDevStackProvider provider(shell);
    auto info = provider.collect_dev_stacks();

    // Contrato: a chamada não deve crashar e deve retornar coleções válidas
    // (tamanhos em faixa razoável; conteúdo depende do sistema do dev).
    EXPECT_LE(info.toolchains.size(), 32u);
    EXPECT_LE(info.containers.size(), 1024u);

    // Quando há toolchain detectada, version deve ser não-vazia (regex garante isso).
    for (const auto& t : info.toolchains) {
        EXPECT_FALSE(t.name.isEmpty());
        EXPECT_FALSE(t.version.isEmpty());
    }

    // Runtime dos containers deve ser sempre preenchido e restrito a Podman/Docker.
    for (const auto& c : info.containers) {
        EXPECT_TRUE(c.runtime == "Podman" || c.runtime == "Docker");
    }
}
