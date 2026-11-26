#pragma once

#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"
#include "domain/vram/interfaces/i_leitor_desktop_file.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <memory>

namespace my_comp::application::vram {

class RegraCategoriaDesktop final : public domain::vram::IRegraClassificacao {
public:
    RegraCategoriaDesktop(std::shared_ptr<domain::vram::ILeitorDesktopFile> leitor,
                          MapaCategoriasDesktop mapa);

    [[nodiscard]] std::optional<domain::vram::ResultadoClassificacao>
        avaliar(const domain::vram::Processo& processo) const override;

private:
    std::shared_ptr<domain::vram::ILeitorDesktopFile> m_leitor;
    MapaCategoriasDesktop                       m_mapa;
};

}
