#include "application/vram/classificacao/regras/regra_categoria_desktop.h"

namespace my_comp::application::vram {

RegraCategoriaDesktop::RegraCategoriaDesktop(
    std::shared_ptr<domain::vram::ILeitorDesktopFile> leitor,
    MapaCategoriasDesktop mapa)
    : m_leitor{std::move(leitor)},
      m_mapa{std::move(mapa)} {}

std::optional<domain::vram::ResultadoClassificacao>
RegraCategoriaDesktop::avaliar(const domain::vram::Processo& processo) const {
    auto categorias = m_leitor->categorias_de(processo);
    auto v = m_mapa.classificar_por(categorias);
    if (!v.has_value()) {
        return std::nullopt;
    }
    return domain::vram::ResultadoClassificacao{
        .categoria = *v,
        .justificativa = domain::vram::Justificativa{
            .regra = QStringLiteral("categoria_desktop"),
            .detalhe = QStringLiteral("categorias=[%1]").arg(categorias.join(QLatin1Char(','))),
        },
    };
}

}
