#pragma once

#include "domain/vram/interfaces/i_leitor_desktop_file.h"

#include <QStringList>

namespace my_comp::infrastructure::vram {

class LeitorDesktopFileXdg final : public domain::vram::ILeitorDesktopFile {
public:
    explicit LeitorDesktopFileXdg(QStringList xdg_dirs = {});

    [[nodiscard]] QStringList
        categorias_de(const domain::vram::Processo& processo) const override;

private:
    QStringList m_xdg_dirs;
};

}
