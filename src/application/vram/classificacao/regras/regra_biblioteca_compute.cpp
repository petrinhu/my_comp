#include "application/vram/classificacao/regras/regra_biblioteca_compute.h"

namespace vramopt::application {

RegraBibliotecaCompute::RegraBibliotecaCompute(
    std::shared_ptr<domain::ILeitorMapsProcesso> leitor,
    ListaLibsCompute lista)
    : m_leitor{std::move(leitor)},
      m_lista{std::move(lista)} {}

std::optional<domain::ResultadoClassificacao>
RegraBibliotecaCompute::avaliar(const domain::Processo& processo) const {
    auto bibliotecas = m_leitor->bibliotecasMapeadas(processo.pid());
    auto casamento = m_lista.primeiraQueCasa(bibliotecas);
    if (!casamento.has_value()) {
        return std::nullopt;
    }
    return domain::ResultadoClassificacao{
        .categoria = domain::Categoria::PrecisaGpu,
        .justificativa = domain::Justificativa{
            .regra = QStringLiteral("biblioteca_compute"),
            .detalhe = QStringLiteral("linka %1").arg(*casamento),
        },
    };
}

}
