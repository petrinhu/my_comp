#include "domain/vram/valueobjects/pid.h"

#include <stdexcept>

namespace my_comp::domain::vram {

PID::PID(underlying_t valor) : m_valor{valor} {
    if (valor <= 0) {
        throw std::invalid_argument{"PID deve ser positivo"};
    }
}

}
