#pragma once

// Catálogo de classificação (VRAM-06) lido de JSON. O JSON é entrada não-confiável
// (MC-10): o parser valida cada entrada e ignora, com qWarning, a que não fecha o
// contrato, sem derrubar a carga. O catálogo empacotado vive em
// src/resources/vram/catalogo.json (registrado em catalogo_vram.qrc).
//
// Formato (versao 1):
//   { "versao": 1,
//     "entradas": [ { "id": "firefox",                     // obrigatório, único
//                     "binarios":   ["firefox"],           // /proc/<pid>/comm, caixa exata
//                     "wm_class":   ["firefox"],           // caixa ignorada
//                     "desktop_id": ["firefox.desktop"],   // caixa ignorada
//                     "veredito": "CandidatoObvioCpu",     // ou "PrecisaGpu"
//                     "justificativa": "..." } ] }         // obrigatório
// Ao menos uma chave (binarios/wm_class/desktop_id) é exigida. "Indeterminado" é
// recusado: o catálogo só afirma vereditos; uma entrada indecisa encerraria a cascata
// sem decidir e calaria as regras seguintes.

#include "domain/vram/entidades/entrada_catalogo.h"
#include "domain/vram/interfaces/i_catalogo_classificacao.h"

#include <QByteArray>
#include <QHash>
#include <QString>

#include <cstddef>
#include <vector>

namespace vramopt::infrastructure {

struct ResultadoCargaCatalogo {
    std::vector<domain::EntradaCatalogo> entradas;
    int                                  rejeitadas{0};
};

// Parser puro (sem I/O): JSON inválido devolve resultado vazio.
[[nodiscard]] ResultadoCargaCatalogo parsear_catalogo_json(const QByteArray& json);

// Lê o recurso :/vram/catalogo.json embarcado no binário.
[[nodiscard]] ResultadoCargaCatalogo carregar_catalogo_empacotado();

class CatalogoClassificacaoJson final : public domain::ICatalogoClassificacao {
public:
    explicit CatalogoClassificacaoJson(std::vector<domain::EntradaCatalogo> entradas);

    [[nodiscard]] std::optional<domain::EntradaCatalogo>
        buscar(const domain::ChaveProcesso& chave) const override;

private:
    std::vector<domain::EntradaCatalogo> m_entradas;
    QHash<QString, std::size_t>          m_por_binario;
    QHash<QString, std::size_t>          m_por_wm_class;    // chave em minúsculas
    QHash<QString, std::size_t>          m_por_desktop_id;  // chave em minúsculas
};

}
