#include "domain/vram/valueobjects/nome_processo.h"

#include <stdexcept>

namespace vramopt::domain {

NomeProcesso::NomeProcesso(QString valor) : m_valor{valor.trimmed()} {
    if (m_valor.isEmpty()) {
        throw std::invalid_argument{"NomeProcesso nao pode ser vazio nem so espacos"};
    }
}

}
