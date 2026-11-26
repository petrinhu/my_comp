#pragma once

#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockRegraClassificacao : public my_comp::domain::vram::IRegraClassificacao {
public:
    MOCK_METHOD(std::optional<my_comp::domain::vram::ResultadoClassificacao>, avaliar,
                (const my_comp::domain::vram::Processo&), (const, override));
};

}
