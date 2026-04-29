#pragma once

#include "back/interfaces/i_config_locator.h"
#include "back/interfaces/i_leitor_environ.h"

#include <memory>

namespace vramopt::application {

class LocalizadorEnvVar final : public domain::IConfigLocator {
public:
    explicit LocalizadorEnvVar(std::shared_ptr<domain::ILeitorEnviron> leitor);

    [[nodiscard]] QVector<domain::OrigemConfig>
        localizar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorEnviron> m_leitor;
};

}
