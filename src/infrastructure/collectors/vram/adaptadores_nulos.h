#pragma once

// Null objects para as regras da cascata cujas fontes de dados ainda não existem.
// Cada um faz a regra correspondente se abster (std::nullopt), sem inventar veredito.
// Catálogo (catalogo_classificacao_json.h), feedback (repositorio_feedback_cifrado.h) e
// amostrador (historico_amostras_vram.h) já têm implementação real; os null objects
// ficam para testes e para montar a cascata só para listar a ordem das regras.

#include "domain/vram/amostra/amostra_janela.h"
#include "domain/vram/interfaces/i_amostrador_comportamento.h"
#include "domain/vram/interfaces/i_catalogo_classificacao.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

namespace vramopt::infrastructure {

class CatalogoClassificacaoVazio final : public domain::ICatalogoClassificacao {
public:
    [[nodiscard]] std::optional<domain::EntradaCatalogo>
        buscar(const domain::ChaveProcesso&) const override { return std::nullopt; }
};

class RepositorioFeedbackVazio final : public domain::IRepositorioFeedback {
public:
    [[nodiscard]] std::optional<domain::EntradaFeedback>
        buscar(const domain::ChaveProcesso&) const override { return std::nullopt; }
    void inserir(const domain::EntradaFeedback&) override {}
    void atualizar_visto_em(int, QDateTime) override {}
    void remover(int) override {}
    [[nodiscard]] QVector<domain::EntradaFeedback> listar_todos() const override { return {}; }
};

class AmostradorComportamentoNulo final : public domain::IAmostradorComportamento {
public:
    [[nodiscard]] domain::AmostraJanela
        amostra_janela(const domain::PID&, std::chrono::seconds janela) const override {
        domain::AmostraJanela a;
        a.janela = janela; // amostras = 0 → RegraComportamento se abstém
        return a;
    }
};

}
