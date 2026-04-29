#pragma once

#include "back/configuracao/origem_config.h"
#include "back/entidades/processo.h"
#include "back/interfaces/i_config_locator.h"

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
