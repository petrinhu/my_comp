#pragma once

#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockRegraClassificacao : public vramopt::domain::IRegraClassificacao {
public:
    MOCK_METHOD(std::optional<vramopt::domain::ResultadoClassificacao>, avaliar,
                (const vramopt::domain::Processo&), (const, override));
};

}
