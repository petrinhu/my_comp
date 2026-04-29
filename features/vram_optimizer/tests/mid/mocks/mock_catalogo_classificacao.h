#pragma once

#include "back/interfaces/i_catalogo_classificacao.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockCatalogoClassificacao : public vramopt::domain::ICatalogoClassificacao {
public:
    MOCK_METHOD(std::optional<vramopt::domain::EntradaCatalogo>, buscar,
                (const vramopt::domain::ChaveProcesso&), (const, override));
};

}
