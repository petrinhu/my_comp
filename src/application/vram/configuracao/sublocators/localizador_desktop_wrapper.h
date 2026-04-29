#pragma once

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <memory>

namespace vramopt::application {

class LocalizadorDesktopWrapper final : public domain::IConfigLocator {
public:
    explicit LocalizadorDesktopWrapper(std::shared_ptr<domain::ILeitorDesktopExec> leitor);

    [[nodiscard]] QVector<domain::OrigemConfig>
        localizar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorDesktopExec> m_leitor;
};

}
