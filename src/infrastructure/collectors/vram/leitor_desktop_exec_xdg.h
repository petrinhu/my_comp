#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_exec.h"

#include <QStringList>

namespace vramopt::infrastructure {

class LeitorDesktopExecXdg final : public domain::ILeitorDesktopExec {
public:
    explicit LeitorDesktopExecXdg(QStringList xdg_dirs = {});

    [[nodiscard]] std::optional<domain::EntradaDesktopExec>
        entrada_de(const domain::Processo& processo) const override;

private:
    QStringList m_xdg_dirs;
};

}
