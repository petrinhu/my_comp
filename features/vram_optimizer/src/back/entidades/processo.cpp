#include "back/entidades/processo.h"

namespace vramopt::domain {

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
