#pragma once

// Extensão do composition root (MC-02: DI manual, providers concretos só aqui e em main.cpp).
// Ponto único de montagem da auditoria de VRAM, usado pela GUI e pelo subcomando CLI.

#include "application/vram/auditoria/servico_auditar_vram_atual.h"
#include "application/vram/feedback/servico_registrar_feedback.h"
#include "domain/vram/interfaces/i_amostrador_comportamento.h"
#include "domain/vram/interfaces/i_catalogo_classificacao.h"
#include "domain/vram/interfaces/i_gpu_backend.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <QString>

#include <memory>
#include <vector>

namespace vramopt::composition {

// Casos de uso da aba VRAM montados sobre as mesmas fontes (o feedback gravado por
// registrar_feedback é o que a RegraFeedback de auditar lê).
struct ServicosVram {
    std::shared_ptr<application::ServicoAuditarVramAtual>  auditar;
    std::shared_ptr<application::ServicoRegistrarFeedback> registrar_feedback;
};

[[nodiscard]] ServicosVram compor_servicos_vram();

// Atalho para quem só audita (subcomando CLI).
[[nodiscard]] std::shared_ptr<application::ServicoAuditarVramAtual> compor_servico_auditar_vram();

// Catálogo usado pela RegraCatalogo: o JSON empacotado em :/vram/catalogo.json (VRAM-06).
[[nodiscard]] std::shared_ptr<domain::ICatalogoClassificacao> compor_catalogo_classificacao();

// Feedback do usuário cifrado via ISecurityVault em AppDataLocation/vram (VRAM-07).
// Construir não toca o disco: a carga é preguiçosa, no primeiro acesso.
[[nodiscard]] std::shared_ptr<domain::IRepositorioFeedback> compor_repositorio_feedback();

// Backend da auditoria + amostrador da RegraComportamento (VRAM-08): cada enumeração de
// processos feita pela auditoria vira uma amostra; a regra agrega a janela sem esperar.
struct BackendComAmostrador {
    std::shared_ptr<domain::IGpuBackend>              backend;
    std::shared_ptr<domain::IAmostradorComportamento> amostrador;
};

[[nodiscard]] BackendComAmostrador compor_backend_com_amostrador(
    std::shared_ptr<domain::IGpuBackend> backend_real);

// Ordem efetiva da cascata de classificação (primeira regra que opina vence).
[[nodiscard]] std::vector<QString> nomes_das_regras_da_cascata();

}
