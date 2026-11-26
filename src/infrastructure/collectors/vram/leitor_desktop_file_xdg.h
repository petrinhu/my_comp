#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_file.h"

#include <QStringList>

namespace vramopt::infrastructure {

class LeitorDesktopFileXdg final : public domain::ILeitorDesktopFile {
public:
    explicit LeitorDesktopFileXdg(QStringList xdg_dirs = {});

    [[nodiscard]] QStringList
        categorias_de(const domain::Processo& processo) const override;

private:
    QStringList m_xdg_dirs;
};

}
