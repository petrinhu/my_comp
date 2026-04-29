#include "back/valueobjects/pid.h"

#include <stdexcept>

namespace vramopt::domain {

PID::PID(underlying_t valor) : m_valor{valor} {
    if (valor <= 0) {
        throw std::invalid_argument{"PID deve ser positivo"};
    }
}

}
