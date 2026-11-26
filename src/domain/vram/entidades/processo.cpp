#include "domain/vram/entidades/processo.h"

namespace my_comp::domain::vram {

Processo::Processo(PID pid,
                   NomeProcesso nome,
                   BytesDeMemoria vram,
                   int gpu_id,
                   std::vector<ChaveProcesso> chaves)
    : m_pid{pid},
      m_nome{std::move(nome)},
      m_vram{vram},
      m_gpu_id{gpu_id},
      m_chaves{std::move(chaves)} {}

}
