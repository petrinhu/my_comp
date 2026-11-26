#include <gtest/gtest.h>
#include <QFile>
#include "../../src/infrastructure/collectors/linux_system_info_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxSystemInfoProviderTest, ShouldCollectSystemInfoFromRealSystem) {
    if (!QFile::exists("/etc/os-release") && !QFile::exists("/usr/lib/os-release")) {
        GTEST_SKIP() << "sistema sem os-release";
    }
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxSystemInfoProvider provider(shell);
    auto info = provider.get_system_info();
    
    EXPECT_FALSE(info.distroName.isEmpty());
    EXPECT_FALSE(info.kernelVersion.isEmpty());
    // Agnóstico a distro (Fedora é o alvo, mas o design é multi-distro): só exige conteúdo.
}
