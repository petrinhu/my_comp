#pragma once

#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/auditoria/servico_auditar_vram_atual.h"

#include <QString>
#include <QStringList>
#include <QTextStream>

#include <memory>

namespace my_comp::presentation::cli {

struct OpcoesCliVram {
    bool json                   = false;
    bool all_processes          = false;
    bool include_config_origins = true;
};

[[nodiscard]] OpcoesCliVram parsear_opcoes(const QStringList& args);

[[nodiscard]] QString formatar_relatorio(
    const application::vram::RelatorioAuditoriaVram& rel,
    const OpcoesCliVram& opts);

// O serviço vem montado do composition root (main.cpp → composition/vram_composition):
// a camada de apresentação não instancia infraestrutura (MC-02).
int executar_com_servico(
    const std::shared_ptr<application::vram::ServicoAuditarVramAtual>& servico,
    const QStringList& args,
    QTextStream& out);

}
