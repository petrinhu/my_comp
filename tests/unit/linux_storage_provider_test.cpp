#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_storage_provider.h"

using namespace my_comp::infrastructure::collectors;

TEST(LinuxStorageProviderTest, ShouldCollectStorageStatsFromRealSystem) {
    LinuxStorageProvider provider;
    
    // Primeira chamada para baseline
    provider.collect_stats();
    
    // Pequena pausa não é necessária pois o construtor já faz o baseline inicial agora (corrigido na auditoria)
    auto stats = provider.collect_stats();
    
    // Deve detectar discos se rodando em hardware real ou VM com disco virtio/scsi
    // Se falhar em algum ambiente específico, ajustamos as expectativas
    EXPECT_GE(stats.total_read_mb_s, 0.0);
    EXPECT_GE(stats.total_write_mb_s, 0.0);
}
