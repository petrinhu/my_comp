#pragma once

#include "domain/vram/interfaces/i_leitor_maps_processo.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorMapsProcesso : public vramopt::domain::ILeitorMapsProcesso {
public:
    MOCK_METHOD(QStringList, bibliotecas_mapeadas,
                (const vramopt::domain::PID&), (const, override));
};

}
