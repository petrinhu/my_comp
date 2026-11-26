#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

#include <gmock/gmock.h>

#include <vector>

namespace my_comp::tests::vram {

class MockGpuBackend : public my_comp::domain::vram::IGpuBackend {
public:
    MOCK_METHOD(std::vector<my_comp::domain::vram::GpuInfo>, enumerar_gpus,
                (), (const, override));
    MOCK_METHOD(std::vector<my_comp::domain::vram::Processo>, enumerar_processos_vram,
                (int), (const, override));
};

}
