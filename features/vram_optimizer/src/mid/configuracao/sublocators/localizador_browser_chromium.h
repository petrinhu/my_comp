#pragma once

#include "back/interfaces/i_config_locator.h"
#include "back/interfaces/i_leitor_arquivo_config.h"

#include <memory>

namespace vramopt::application {

class LocalizadorBrowserChromium final : public domain::IConfigLocator {
public:
    explicit LocalizadorBrowserChromium(std::shared_ptr<domain::ILeitorArquivoConfig> leitor);

    [[nodiscard]] QVector<domain::OrigemConfig>
        localizar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorArquivoConfig> m_leitor;
};

}
