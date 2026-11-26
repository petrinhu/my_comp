#pragma once

#include "domain/vram/valueobjects/pid.h"

#include <QStringList>

namespace my_comp::domain::vram {

class ILeitorMapsProcesso {
public:
    ILeitorMapsProcesso() = default;
    ILeitorMapsProcesso(const ILeitorMapsProcesso&) = delete;
    ILeitorMapsProcesso(ILeitorMapsProcesso&&) = delete;
    ILeitorMapsProcesso& operator=(const ILeitorMapsProcesso&) = delete;
    ILeitorMapsProcesso& operator=(ILeitorMapsProcesso&&) = delete;
    virtual ~ILeitorMapsProcesso() = default;

    [[nodiscard]] virtual QStringList
        bibliotecas_mapeadas(const PID& pid) const = 0;
};

}
