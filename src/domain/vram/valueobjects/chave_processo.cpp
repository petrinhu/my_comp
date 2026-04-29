#include "domain/vram/valueobjects/chave_processo.h"

#include <stdexcept>

namespace vramopt::domain {

ChaveProcesso::ChaveProcesso(TipoChave tipo, QString valor)
    : m_tipo{tipo}, m_valor{std::move(valor)} {
    if (m_valor.isEmpty()) {
        throw std::invalid_argument{"ChaveProcesso valor nao pode ser vazio"};
    }
}

ChaveProcesso ChaveProcesso::deBinario(QString valor) {
    return ChaveProcesso{TipoChave::Binario, std::move(valor)};
}
ChaveProcesso ChaveProcesso::deWmClass(QString valor) {
    return ChaveProcesso{TipoChave::WmClass, std::move(valor)};
}
ChaveProcesso ChaveProcesso::deDesktopId(QString valor) {
    return ChaveProcesso{TipoChave::DesktopId, std::move(valor)};
}

}
