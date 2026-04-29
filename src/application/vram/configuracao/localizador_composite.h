#pragma once

#include "domain/vram/interfaces/i_config_locator.h"

#include <memory>
#include <vector>

namespace vramopt::application {

class LocalizadorComposite final : public domain::IConfigLocator {
public:
    explicit LocalizadorComposite(std::vector<std::shared_ptr<domain::IConfigLocator>> sublocators);

    [[nodiscard]] QVector<domain::OrigemConfig>
        localizar(const domain::Processo& processo) const override;

private:
    std::vector<std::shared_ptr<domain::IConfigLocator>> m_sublocators;
};

}
