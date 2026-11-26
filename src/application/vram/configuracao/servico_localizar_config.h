#pragma once

#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/interfaces/i_config_locator.h"

#include <QVector>

#include <memory>

namespace my_comp::application::vram {

class ServicoLocalizarConfig {
public:
    explicit ServicoLocalizarConfig(std::shared_ptr<domain::vram::IConfigLocator> locator);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        executar(const domain::vram::Processo& processo) const;

private:
    std::shared_ptr<domain::vram::IConfigLocator> m_locator;
};

}
