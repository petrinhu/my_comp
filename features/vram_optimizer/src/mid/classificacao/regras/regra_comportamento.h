#pragma once

#include "back/amostra/limiares_comportamento.h"
#include "back/interfaces/i_amostrador_comportamento.h"
#include "back/interfaces/i_regra_classificacao.h"

#include <chrono>
#include <memory>

namespace vramopt::application {

class RegraComportamento final : public domain::IRegraClassificacao {
public:
    RegraComportamento(std::shared_ptr<domain::IAmostradorComportamento> amostrador,
                       domain::LimiaresComportamento                     limiares,
                       std::chrono::seconds                              janela);

    [[nodiscard]] std::optional<domain::ResultadoClassificacao>
        avaliar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::IAmostradorComportamento> m_amostrador;
    domain::LimiaresComportamento                     m_limiares;
    std::chrono::seconds                              m_janela;
};

}
