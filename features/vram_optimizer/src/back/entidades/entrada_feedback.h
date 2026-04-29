#pragma once

#include "back/classificacao/categoria.h"
#include "back/valueobjects/chave_processo.h"

#include <QDateTime>
#include <QString>

#include <optional>

namespace vramopt::domain {

struct EntradaFeedback {
    int                       id{0};
    ChaveProcesso             chave;        // sem default: ChaveProcesso só tem fábricas estáticas
    Categoria                 veredito{Categoria::Indeterminado};
    QString                   motivo{};
    QDateTime                 criado_em{};
    QDateTime                 visto_em{};
    std::optional<QDateTime>  revisado_em{};
};

}
