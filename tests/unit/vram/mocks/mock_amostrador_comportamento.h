#pragma once

#include "domain/vram/interfaces/i_amostrador_comportamento.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockAmostradorComportamento : public my_comp::domain::vram::IAmostradorComportamento {
public:
    MOCK_METHOD(my_comp::domain::vram::AmostraJanela, amostra_janela,
                (const my_comp::domain::vram::PID&, std::chrono::seconds), (const, override));
};

}
