#pragma once

#include "domain/vram/amostra/limiares_comportamento.h"
#include "domain/vram/interfaces/i_amostrador_comportamento.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <chrono>
#include <memory>

namespace my_comp::application::vram {

class RegraComportamento final : public domain::vram::IRegraClassificacao {
public:
    RegraComportamento(std::shared_ptr<domain::vram::IAmostradorComportamento> amostrador,
                       domain::vram::LimiaresComportamento                     limiares,
                       std::chrono::seconds                              janela);

    [[nodiscard]] std::optional<domain::vram::ResultadoClassificacao>
        avaliar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::IAmostradorComportamento> m_amostrador;
    domain::vram::LimiaresComportamento                     m_limiares;
    std::chrono::seconds                              m_janela;
};

}
