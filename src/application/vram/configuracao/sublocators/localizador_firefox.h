#pragma once

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_leitor_arquivo_config.h"

#include <memory>

namespace my_comp::application::vram {

class LocalizadorFirefox final : public domain::vram::IConfigLocator {
public:
    explicit LocalizadorFirefox(std::shared_ptr<domain::vram::ILeitorArquivoConfig> leitor);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        localizar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorArquivoConfig> m_leitor;
};

}
