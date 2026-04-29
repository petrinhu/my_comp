#pragma once

#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_config_locator.h"

#include <QVector>

#include <memory>

namespace vramopt::application {

class ServicoLocalizarConfig {
public:
    explicit ServicoLocalizarConfig(std::shared_ptr<domain::IConfigLocator> locator);

    [[nodiscard]] QVector<domain::OrigemConfig>
        executar(const domain::Processo& processo) const;

private:
    std::shared_ptr<domain::IConfigLocator> m_locator;
};

}
