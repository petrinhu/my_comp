#include "back/entidades/gpu_info.h"

namespace vramopt::domain {

GpuInfo::GpuInfo(int id, Fabricante fab, QString modelo, BytesDeMemoria vram_total)
    : m_id{id},
      m_fabricante{fab},
      m_modelo{std::move(modelo)},
      m_vram_total{vram_total} {}

}
