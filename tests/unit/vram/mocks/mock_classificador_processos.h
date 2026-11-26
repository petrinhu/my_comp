#pragma once

#include "domain/vram/interfaces/i_classificador_processos.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockClassificadorProcessos : public my_comp::domain::vram::IClassificadorProcessos {
public:
    MOCK_METHOD(my_comp::domain::vram::ResultadoClassificacao, classificar,
                (const my_comp::domain::vram::Processo&), (const, override));
};

}
