#pragma once

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <QDateTime>
#include <QString>

#include <cstddef>
#include <functional>
#include <memory>

namespace my_comp::application::vram {

// Caso de uso: o usuário fixa o veredito de um processo (VRAM-07). O feedback vence
// as demais regras da cascata na próxima auditoria (RegraFeedback é a primeira).
class ServicoRegistrarFeedback {
public:
    using Relogio = std::function<QDateTime()>;

    static constexpr std::size_t TAMANHO_MAXIMO_MOTIVO = 500;

    // Relógio vazio usa QDateTime::currentDateTimeUtc().
    explicit ServicoRegistrarFeedback(std::shared_ptr<domain::vram::IRepositorioFeedback> repo,
                                      Relogio relogio = {});

    // Indeterminado não fixa veredito: devolve false sem gravar. Revisar um feedback
    // existente mantém id e criado_em e marca revisado_em.
    [[nodiscard]] bool executar(const domain::vram::ChaveProcesso& chave,
                                domain::vram::Categoria veredito,
                                const QString& motivo) const;

private:
    std::shared_ptr<domain::vram::IRepositorioFeedback> m_repo;
    Relogio                                       m_relogio;
};

}
