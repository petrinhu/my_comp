#pragma once

#include <QString>

#include <cstdint>

namespace vramopt::application {

enum class TipoSugestao : std::uint8_t {
    Nenhuma,
    AjustarConfigDoApp,
    AvaliacaoManualNecessaria,
};

struct SugestaoAcao {
    TipoSugestao tipo;
    QString      descricao;
};

}
