#include "application/vram/classificacao/regras/regra_comportamento.h"

#include <QString>

namespace my_comp::application::vram {

namespace {

QString detalhe_da_amostra(const domain::vram::AmostraJanela& a) {
    return QStringLiteral("VRAM %1%/total, compute %2% media, janela %3s")
        .arg(a.razao_vram_sobre_total() * 100.0, 0, 'f', 1)
        .arg(a.compute_utilizacao_media * 100.0, 0, 'f', 1)
        .arg(a.janela.count());
}

}

RegraComportamento::RegraComportamento(
    std::shared_ptr<domain::vram::IAmostradorComportamento> amostrador,
    domain::vram::LimiaresComportamento limiares,
    std::chrono::seconds janela)
    : m_amostrador{std::move(amostrador)},
      m_limiares{limiares},
      m_janela{janela} {}

std::optional<domain::vram::ResultadoClassificacao>
RegraComportamento::avaliar(const domain::vram::Processo& processo) const {
    const auto a = m_amostrador->amostra_janela(processo.pid(), m_janela);
    if (a.amostras <= 0) {
        return std::nullopt; // sem dados não há veredito; zeros pareceriam "trivial + ocioso"
    }
    if (!a.compute_medido) {
        // Só VRAM conhecida: compute 0.0 é ausência de medida, não ociosidade. Todas as
        // linhas abaixo dependem do compute, então decidir aqui seria inventar veredito.
        return std::nullopt;
    }

    const bool compute_sustentado = a.compute_utilizacao_media >= m_limiares.compute_sustentado;
    const auto vram_alta_efet     = m_limiares.vram_alta_efetivo(a.vram_total_gpu);
    const auto vram_trivial_efet  = m_limiares.vram_trivial_efetivo(a.vram_total_gpu);
    const bool vram_alta          = a.vram_media >= vram_alta_efet;
    const bool vram_trivial       = a.vram_media <= vram_trivial_efet;
    const bool compute_idle       = a.compute_utilizacao_media < m_limiares.compute_idle;

    const domain::vram::Justificativa just{
        .regra = QStringLiteral("comportamento"),
        .detalhe = detalhe_da_amostra(a),
    };

    // Linha 1: compute alto sustentado → PrecisaGpu
    if (compute_sustentado) {
        return domain::vram::ResultadoClassificacao{.categoria = domain::vram::Categoria::PrecisaGpu, .justificativa = just};
    }
    // Linha 2: VRAM alta + alguma atividade → PrecisaGpu
    if (vram_alta && !compute_idle) {
        return domain::vram::ResultadoClassificacao{.categoria = domain::vram::Categoria::PrecisaGpu, .justificativa = just};
    }
    // Linha 3: VRAM trivial + idle → CandidatoObvioCpu
    if (vram_trivial && compute_idle) {
        return domain::vram::ResultadoClassificacao{.categoria = domain::vram::Categoria::CandidatoObvioCpu, .justificativa = just};
    }
    // Linha 4: resto → Indeterminado
    return domain::vram::ResultadoClassificacao{.categoria = domain::vram::Categoria::Indeterminado, .justificativa = just};
}

}
