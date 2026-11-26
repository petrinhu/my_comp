#include "application/vram/feedback/servico_registrar_feedback.h"

#include <utility>

namespace my_comp::application::vram {

ServicoRegistrarFeedback::ServicoRegistrarFeedback(
    std::shared_ptr<domain::vram::IRepositorioFeedback> repo, Relogio relogio)
    : m_repo{std::move(repo)}, m_relogio{std::move(relogio)} {
    if (!m_relogio) {
        m_relogio = [] { return QDateTime::currentDateTimeUtc(); };
    }
}

bool ServicoRegistrarFeedback::executar(const domain::vram::ChaveProcesso& chave,
                                        domain::vram::Categoria veredito,
                                        const QString& motivo) const {
    if (!m_repo || veredito == domain::vram::Categoria::Indeterminado) {
        return false;
    }
    const QDateTime agora = m_relogio();
    domain::vram::EntradaFeedback entrada{
        .id = 0,
        .chave = chave,
        .veredito = veredito,
        .motivo = motivo.trimmed().left(static_cast<qsizetype>(TAMANHO_MAXIMO_MOTIVO)),
        .criado_em = agora,
        .visto_em = agora,
        .revisado_em = std::nullopt,
    };
    if (const auto anterior = m_repo->buscar(chave); anterior.has_value()) {
        entrada.id = anterior->id;
        entrada.criado_em = anterior->criado_em;
        entrada.revisado_em = agora;
    }
    m_repo->inserir(entrada);
    return true;
}

}
