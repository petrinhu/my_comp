#pragma once

#include "back/valueobjects/bytes_de_memoria.h"

#include <cstdint>
#include <QString>

namespace vramopt::domain {

enum class Fabricante : std::uint8_t { Nvidia, Intel, Amd, Desconhecido };

class GpuInfo {
public:
    GpuInfo(int id, Fabricante fab, QString modelo, BytesDeMemoria vram_total);

    [[nodiscard]] int             id() const noexcept { return m_id; }
    [[nodiscard]] Fabricante      fabricante() const noexcept { return m_fabricante; }
    [[nodiscard]] const QString&  modelo() const noexcept { return m_modelo; }
    [[nodiscard]] BytesDeMemoria  vramTotal() const noexcept { return m_vram_total; }

private:
    int            m_id;
    Fabricante     m_fabricante;
    QString        m_modelo;
    BytesDeMemoria m_vram_total;
};

}
