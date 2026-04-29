#pragma once

#include "mid/classificacao/regras/lista_libs_compute.h"
#include "back/interfaces/i_leitor_maps_processo.h"
#include "back/interfaces/i_regra_classificacao.h"

#include <memory>

namespace vramopt::application {

class RegraBibliotecaCompute final : public domain::IRegraClassificacao {
public:
    RegraBibliotecaCompute(std::shared_ptr<domain::ILeitorMapsProcesso> leitor,
                           ListaLibsCompute lista);

    [[nodiscard]] std::optional<domain::ResultadoClassificacao>
        avaliar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorMapsProcesso> m_leitor;
    ListaLibsCompute                             m_lista;
};

}
