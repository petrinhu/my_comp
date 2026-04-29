#pragma once

#include "back/interfaces/i_config_locator.h"
#include "back/interfaces/i_leitor_arquivo_config.h"

#include <memory>

namespace vramopt::application {

class LocalizadorFirefox final : public domain::IConfigLocator {
public:
    explicit LocalizadorFirefox(std::shared_ptr<domain::ILeitorArquivoConfig> leitor);

    [[nodiscard]] QVector<domain::OrigemConfig>
        localizar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorArquivoConfig> m_leitor;
};

}
