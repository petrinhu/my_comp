#pragma once

#include "back/interfaces/i_leitor_arquivo_config.h"

#include <gmock/gmock.h>

namespace vramopt::tests {

class MockLeitorArquivoConfig : public vramopt::domain::ILeitorArquivoConfig {
public:
    MOCK_METHOD(std::optional<QByteArray>, ler, (const QString& path), (const, override));
};

}
