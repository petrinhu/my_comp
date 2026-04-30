#pragma once

#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/auditoria/servico_auditar_vram_atual.h"

#include <QString>
#include <QStringList>
#include <QTextStream>

#include <memory>

namespace vramopt::presentation::cli {

struct OpcoesCliVram {
    bool json                   = false;
    bool all_processes          = false;
    bool include_config_origins = true;
};

[[nodiscard]] OpcoesCliVram parsearOpcoes(const QStringList& args);

[[nodiscard]] QString formatarRelatorio(
    const application::RelatorioAuditoriaVram& rel,
    const OpcoesCliVram& opts);

int executarComServico(
    const std::shared_ptr<application::ServicoAuditarVramAtual>& servico,
    const QStringList& args,
    QTextStream& out);

int executar(const QStringList& args, QTextStream& out);

}
