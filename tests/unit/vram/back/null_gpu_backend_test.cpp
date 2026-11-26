#include "infrastructure/collectors/vram/null_gpu_backend.h"

#include <gtest/gtest.h>

using my_comp::infrastructure::vram::NullGpuBackend;

TEST(NullGpuBackendTest, NaoEnumeraGpus) {
    NullGpuBackend backend;
    EXPECT_TRUE(backend.enumerar_gpus().empty());
}

TEST(NullGpuBackendTest, NaoEnumeraProcessos) {
    NullGpuBackend backend;
    EXPECT_TRUE(backend.enumerar_processos_vram(0).empty());
    EXPECT_TRUE(backend.enumerar_processos_vram(99).empty());
}
