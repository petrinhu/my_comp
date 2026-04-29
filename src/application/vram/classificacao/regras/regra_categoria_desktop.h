#pragma once

#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"
#include "domain/vram/interfaces/i_leitor_desktop_file.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"

#include <memory>

namespace vramopt::application {

class RegraCategoriaDesktop final : public domain::IRegraClassificacao {
public:
    RegraCategoriaDesktop(std::shared_ptr<domain::ILeitorDesktopFile> leitor,
                          MapaCategoriasDesktop mapa);

    [[nodiscard]] std::optional<domain::ResultadoClassificacao>
        avaliar(const domain::Processo& processo) const override;

private:
    std::shared_ptr<domain::ILeitorDesktopFile> m_leitor;
    MapaCategoriasDesktop                       m_mapa;
};

}
