#include "domain/vram/valueobjects/bytes_de_memoria.h"

namespace my_comp::domain::vram {

BytesDeMemoria BytesDeMemoria::de_megabytes(underlying_t mb) noexcept {
    return BytesDeMemoria{mb * 1024 * 1024};
}

BytesDeMemoria BytesDeMemoria::de_kilobytes(underlying_t kb) noexcept {
    return BytesDeMemoria{kb * 1024};
}

double BytesDeMemoria::razao_sobre(BytesDeMemoria total) const noexcept {
    if (total.m_valor == 0) {
        return 0.0;
    }
    return static_cast<double>(m_valor) / static_cast<double>(total.m_valor);
}

}
