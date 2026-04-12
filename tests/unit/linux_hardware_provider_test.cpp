#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxHardwareProviderTest, ShouldCollectHardwareTopologyFromRealSystem) {
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxHardwareProvider provider(shell);
    auto topology = provider.collect_topology();
    
    // SMBIOS/DMI deve ter pelo menos alguns campos (Vendor/Model) na maioria dos hardwares
    // Em containers pode ser "Unknown", o que é um valor válido retornado pela classe.
    EXPECT_FALSE(topology.smbios.vendor.isEmpty());
    
    // Deve listar USBs se lsusb funcionar (geralmente sim no Fedora)
    // Se falhar em CI, o vetor pode estar vazio, mas a chamada não deve crashar.
    
    // Bluetooth pode estar vazio se não houver adapter, ok.
}
