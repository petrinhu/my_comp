#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_system_info_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxSystemInfoProviderTest, ShouldCollectSystemInfoFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxSystemInfoProvider provider(shell);
    auto info = provider.get_system_info();
    
    EXPECT_FALSE(info.distroName.isEmpty());
    EXPECT_FALSE(info.kernelVersion.isEmpty());
    
    // No Fedora, distroName deve conter "Fedora"
    EXPECT_TRUE(info.distroName.contains("Fedora", Qt::CaseInsensitive) || 
                info.distroName.contains("Linux", Qt::CaseInsensitive));
}
