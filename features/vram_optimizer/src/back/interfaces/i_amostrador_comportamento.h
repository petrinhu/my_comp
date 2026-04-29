#pragma once

#include "back/amostra/amostra_janela.h"
#include "back/valueobjects/pid.h"

#include <chrono>

namespace vramopt::domain {

class IAmostradorComportamento {
public:
    IAmostradorComportamento() = default;
    IAmostradorComportamento(const IAmostradorComportamento&) = delete;
    IAmostradorComportamento(IAmostradorComportamento&&) = delete;
    IAmostradorComportamento& operator=(const IAmostradorComportamento&) = delete;
    IAmostradorComportamento& operator=(IAmostradorComportamento&&) = delete;
    virtual ~IAmostradorComportamento() = default;

    [[nodiscard]] virtual AmostraJanela
        amostraJanela(const PID& pid, std::chrono::seconds janela) const = 0;
};

}
