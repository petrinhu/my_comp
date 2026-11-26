#include "application/vram/classificacao/regras/regra_biblioteca_compute.h"

namespace my_comp::application::vram {

RegraBibliotecaCompute::RegraBibliotecaCompute(
    std::shared_ptr<domain::vram::ILeitorMapsProcesso> leitor,
    ListaLibsCompute lista)
    : m_leitor{std::move(leitor)},
      m_lista{std::move(lista)} {}

std::optional<domain::vram::ResultadoClassificacao>
RegraBibliotecaCompute::avaliar(const domain::vram::Processo& processo) const {
    auto bibliotecas = m_leitor->bibliotecas_mapeadas(processo.pid());
    auto casamento = m_lista.primeira_que_casa(bibliotecas);
    if (!casamento.has_value()) {
        return std::nullopt;
    }
    return domain::vram::ResultadoClassificacao{
        .categoria = domain::vram::Categoria::PrecisaGpu,
        .justificativa = domain::vram::Justificativa{
            .regra = QStringLiteral("biblioteca_compute"),
            .detalhe = QStringLiteral("linka %1").arg(*casamento),
        },
    };
}

}
