#pragma once

#include "domain/vram/interfaces/i_classificador_processos.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockClassificadorProcessos : public vramopt::domain::IClassificadorProcessos {
public:
    MOCK_METHOD(vramopt::domain::ResultadoClassificacao, classificar,
                (const vramopt::domain::Processo&), (const, override));
};

}
