#pragma once

#include "back/interfaces/i_amostrador_comportamento.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockAmostradorComportamento : public vramopt::domain::IAmostradorComportamento {
public:
    MOCK_METHOD(vramopt::domain::AmostraJanela, amostraJanela,
                (const vramopt::domain::PID&, std::chrono::seconds), (const, override));
};

}
