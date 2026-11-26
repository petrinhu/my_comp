#pragma once

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <memory>

namespace my_comp::application::vram {

class LocalizadorDesktopWrapper final : public domain::vram::IConfigLocator {
public:
    explicit LocalizadorDesktopWrapper(std::shared_ptr<domain::vram::ILeitorDesktopExec> leitor);

    [[nodiscard]] QVector<domain::vram::OrigemConfig>
        localizar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorDesktopExec> m_leitor;
};

}
