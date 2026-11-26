#pragma once

#include <QString>

#include <cstdint>

namespace my_comp::domain::vram {

enum class CategoriaOrigem : std::uint8_t {
    BrowserChromium,
    Firefox,
    PrimeEnv,
    DesktopWrapper,
};

enum class Confianca : std::uint8_t { Baixa, Media, Alta };

struct OrigemConfig {
    CategoriaOrigem categoria;
    QString         tipo;
    QString         caminho;
    QString         chave;
    QString         valor_atual;
    QString         valor_sugerido;
    QString         instrucao_humana;
    Confianca       confianca;
    bool            requer_reinicio_app;
    bool            aplicavel_automatico;
};

}
