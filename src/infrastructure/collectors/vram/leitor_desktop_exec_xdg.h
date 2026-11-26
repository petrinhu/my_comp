#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <QStringList>

namespace my_comp::infrastructure::vram {

class LeitorDesktopExecXdg final : public domain::vram::ILeitorDesktopExec {
public:
    explicit LeitorDesktopExecXdg(QStringList xdg_dirs = {});

    [[nodiscard]] std::optional<domain::vram::EntradaDesktopExec>
        entrada_de(const domain::vram::Processo& processo) const override;

private:
    QStringList m_xdg_dirs;
};

}
