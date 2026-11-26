#pragma once

#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <vector>

namespace vramopt::domain {

class Processo {
public:
    Processo(PID pid,
             NomeProcesso nome,
             BytesDeMemoria vram,
             int gpu_id,
             std::vector<ChaveProcesso> chaves);

    [[nodiscard]] const PID&             pid()  const noexcept { return m_pid; }
    [[nodiscard]] const NomeProcesso&    nome() const noexcept { return m_nome; }
    [[nodiscard]] BytesDeMemoria         vram_consumida() const noexcept { return m_vram; }
    [[nodiscard]] int                    gpu_id() const noexcept { return m_gpu_id; }
    [[nodiscard]] const std::vector<ChaveProcesso>& chaves() const noexcept { return m_chaves; }

private:
    PID                       m_pid;
    NomeProcesso              m_nome;
    BytesDeMemoria            m_vram;
    int                       m_gpu_id;
    std::vector<ChaveProcesso> m_chaves;
};

}
