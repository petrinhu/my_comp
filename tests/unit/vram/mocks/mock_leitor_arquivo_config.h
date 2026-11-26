#pragma once

#include "domain/vram/interfaces/i_leitor_arquivo_config.h"

#include <gmock/gmock.h>

namespace my_comp::tests::vram {

class MockLeitorArquivoConfig : public my_comp::domain::vram::ILeitorArquivoConfig {
public:
    MOCK_METHOD(std::optional<QByteArray>, ler, (const QString& path), (const, override));
};

}
