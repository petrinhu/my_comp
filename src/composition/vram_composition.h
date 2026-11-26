#pragma once

// Extensão do composition root (MC-02: DI manual, providers concretos só aqui e em main.cpp).
// Ponto único de montagem da auditoria de VRAM, usado pela GUI e pelo subcomando CLI.

#include "application/vram/auditoria/servico_auditar_vram_atual.h"
#include "domain/vram/interfaces/i_catalogo_classificacao.h"

#include <QString>

#include <memory>
#include <vector>

namespace vramopt::composition {

[[nodiscard]] std::shared_ptr<application::ServicoAuditarVramAtual> compor_servico_auditar_vram();

// Catálogo usado pela RegraCatalogo: o JSON empacotado em :/vram/catalogo.json (VRAM-06).
[[nodiscard]] std::shared_ptr<domain::ICatalogoClassificacao> compor_catalogo_classificacao();

// Ordem efetiva da cascata de classificação (primeira regra que opina vence).
[[nodiscard]] std::vector<QString> nomes_das_regras_da_cascata();

}
