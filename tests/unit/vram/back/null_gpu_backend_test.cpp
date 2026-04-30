#include "infrastructure/collectors/vram/null_gpu_backend.h"

#include <gtest/gtest.h>

using vramopt::infrastructure::NullGpuBackend;

TEST(NullGpuBackendTest, NaoEnumeraGpus) {
    NullGpuBackend backend;
    EXPECT_TRUE(backend.enumerarGpus().empty());
}

TEST(NullGpuBackendTest, NaoEnumeraProcessos) {
    NullGpuBackend backend;
    EXPECT_TRUE(backend.enumerarProcessosVram(0).empty());
    EXPECT_TRUE(backend.enumerarProcessosVram(99).empty());
}
