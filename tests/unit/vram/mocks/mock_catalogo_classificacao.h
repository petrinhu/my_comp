#pragma once

#include "domain/vram/interfaces/i_catalogo_classificacao.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockCatalogoClassificacao : public my_comp::domain::vram::ICatalogoClassificacao {
public:
    MOCK_METHOD(std::optional<my_comp::domain::vram::EntradaCatalogo>, buscar,
                (const my_comp::domain::vram::ChaveProcesso&), (const, override));
};

}
