#pragma once

#include "application/vram/classificacao/regras/lista_libs_compute.h"
#include "domain/vram/interfaces/i_leitor_maps_processo.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <memory>

namespace my_comp::application::vram {

class RegraBibliotecaCompute final : public domain::vram::IRegraClassificacao {
public:
    RegraBibliotecaCompute(std::shared_ptr<domain::vram::ILeitorMapsProcesso> leitor,
                           ListaLibsCompute lista);

    [[nodiscard]] std::optional<domain::vram::ResultadoClassificacao>
        avaliar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorMapsProcesso> m_leitor;
    ListaLibsCompute                             m_lista;
};

}
