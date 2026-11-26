#pragma once

#include "domain/vram/entidades/entrada_feedback.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <QDateTime>
#include <QVector>

#include <optional>

namespace vramopt::domain {

class IRepositorioFeedback {
public:
    IRepositorioFeedback() = default;
    IRepositorioFeedback(const IRepositorioFeedback&) = delete;
    IRepositorioFeedback(IRepositorioFeedback&&) = delete;
    IRepositorioFeedback& operator=(const IRepositorioFeedback&) = delete;
    IRepositorioFeedback& operator=(IRepositorioFeedback&&) = delete;
    virtual ~IRepositorioFeedback() = default;

    [[nodiscard]] virtual std::optional<EntradaFeedback>
        buscar(const ChaveProcesso& chave) const = 0;

    virtual void inserir(const EntradaFeedback& entrada) = 0;
    virtual void atualizar_visto_em(int id, QDateTime quando) = 0;
    virtual void remover(int id) = 0;

    [[nodiscard]] virtual QVector<EntradaFeedback> listar_todos() const = 0;
};

}
