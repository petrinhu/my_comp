#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

#include <gmock/gmock.h>

#include <vector>

namespace vramopt::tests {

class MockGpuBackend : public vramopt::domain::IGpuBackend {
public:
    MOCK_METHOD(std::vector<vramopt::domain::GpuInfo>, enumerarGpus,
                (), (const, override));
    MOCK_METHOD(std::vector<vramopt::domain::Processo>, enumerarProcessosVram,
                (int), (const, override));
};

}
