#pragma once

#include "domain/vram/entidades/processo.h"

#include <QStringList>

namespace my_comp::domain::vram {

class ILeitorDesktopFile {
public:
    ILeitorDesktopFile() = default;
    ILeitorDesktopFile(const ILeitorDesktopFile&) = delete;
    ILeitorDesktopFile(ILeitorDesktopFile&&) = delete;
    ILeitorDesktopFile& operator=(const ILeitorDesktopFile&) = delete;
    ILeitorDesktopFile& operator=(ILeitorDesktopFile&&) = delete;
    virtual ~ILeitorDesktopFile() = default;

    [[nodiscard]] virtual QStringList
        categorias_de(const Processo& processo) const = 0;
};

}
