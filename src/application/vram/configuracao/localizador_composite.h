#pragma once

#include "domain/vram/interfaces/i_config_locator.h"

#include <memory>
#include <vector>

namespace my_comp::application::vram {

class LocalizadorComposite final : public domain::vram::IConfigLocator {
public:
    explicit LocalizadorComposite(std::vector<std::shared_ptr<domain::vram::IConfigLocator>> sublocators);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        localizar(const domain::vram::Processo& processo) const override;

private:
    std::vector<std::shared_ptr<domain::vram::IConfigLocator>> m_sublocators;
};

}
