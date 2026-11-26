#include <gtest/gtest.h>
#include <QDir>
#include "../../src/infrastructure/collectors/linux_hardware_provider.h"
#include "../../src/infrastructure/collectors/qprocess_shell_executor.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxHardwareProviderTest, ShouldCollectHardwareTopologyFromRealSystem) {
    // Containers e algumas VMs não expõem DMI; desde o HW-01b campo ausente fica vazio
    // (antes virava "Unknown" e o teste passava sem dado real).
    if (!QDir("/sys/class/dmi/id").exists()) GTEST_SKIP() << "sem /sys/class/dmi/id";
    auto shell = std::make_shared<QProcessShellExecutor>();
    LinuxHardwareProvider provider(shell);
    auto topology = provider.collect_topology();

    // Hardware real identifica ao menos fabricante/modelo do sistema ou da placa
    // (placeholders "To Be Filled By O.E.M." contam como ausentes).
    const auto& s = topology.smbios;
    EXPECT_FALSE(s.vendor.isEmpty() && s.model.isEmpty() && s.boardVendor.isEmpty() && s.boardName.isEmpty());
    
    // Deve listar USBs se lsusb funcionar (geralmente sim no Fedora)
    // Se falhar em CI, o vetor pode estar vazio, mas a chamada não deve crashar.
    
    // Bluetooth pode estar vazio se não houver adapter, ok.
}
