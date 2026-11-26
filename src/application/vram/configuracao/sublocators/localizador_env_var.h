#pragma once

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_leitor_environ.h"

#include <memory>

namespace my_comp::application::vram {

class LocalizadorEnvVar final : public domain::vram::IConfigLocator {
public:
    explicit LocalizadorEnvVar(std::shared_ptr<domain::vram::ILeitorEnviron> leitor);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        localizar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorEnviron> m_leitor;
};

}
