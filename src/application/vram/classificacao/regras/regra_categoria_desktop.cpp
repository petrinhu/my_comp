#include "application/vram/classificacao/regras/regra_categoria_desktop.h"

namespace vramopt::application {

RegraCategoriaDesktop::RegraCategoriaDesktop(
    std::shared_ptr<domain::ILeitorDesktopFile> leitor,
    MapaCategoriasDesktop mapa)
    : m_leitor{std::move(leitor)},
      m_mapa{std::move(mapa)} {}

std::optional<domain::ResultadoClassificacao>
RegraCategoriaDesktop::avaliar(const domain::Processo& processo) const {
    auto categorias = m_leitor->categoriasDe(processo);
    auto v = m_mapa.classificarPor(categorias);
    if (!v.has_value()) {
        return std::nullopt;
    }
    return domain::ResultadoClassificacao{
        .categoria = *v,
        .justificativa = domain::Justificativa{
            .regra = QStringLiteral("categoria_desktop"),
            .detalhe = QStringLiteral("categorias=[%1]").arg(categorias.join(QLatin1Char(','))),
        },
    };
}

}
