#pragma once

#include "back/valueobjects/bytes_de_memoria.h"
#include "back/valueobjects/chave_processo.h"
#include "back/valueobjects/nome_processo.h"
#include "back/valueobjects/pid.h"

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
    [[nodiscard]] BytesDeMemoria         vramConsumida() const noexcept { return m_vram; }
    [[nodiscard]] int                    gpuId() const noexcept { return m_gpu_id; }
    [[nodiscard]] const std::vector<ChaveProcesso>& chaves() const noexcept { return m_chaves; }

private:
    PID                       m_pid;
    NomeProcesso              m_nome;
    BytesDeMemoria            m_vram;
    int                       m_gpu_id;
    std::vector<ChaveProcesso> m_chaves;
};

}
