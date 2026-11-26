#pragma once

#include "domain/vram/interfaces/i_leitor_maps_processo.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockLeitorMapsProcesso : public my_comp::domain::vram::ILeitorMapsProcesso {
public:
    MOCK_METHOD(QStringList, bibliotecas_mapeadas,
                (const my_comp::domain::vram::PID&), (const, override));
};

}
