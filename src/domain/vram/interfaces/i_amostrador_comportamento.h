#pragma once

#include "domain/vram/amostra/amostra_janela.h"
#include "domain/vram/valueobjects/pid.h"

#include <chrono>

namespace my_comp::domain::vram {

class IAmostradorComportamento {
public:
    IAmostradorComportamento() = default;
    IAmostradorComportamento(const IAmostradorComportamento&) = delete;
    IAmostradorComportamento(IAmostradorComportamento&&) = delete;
    IAmostradorComportamento& operator=(const IAmostradorComportamento&) = delete;
    IAmostradorComportamento& operator=(IAmostradorComportamento&&) = delete;
    virtual ~IAmostradorComportamento() = default;

    [[nodiscard]] virtual AmostraJanela
        amostra_janela(const PID& pid, std::chrono::seconds janela) const = 0;
};

}
