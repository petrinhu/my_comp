#pragma once

#include <cstdint>

#include <QString>

namespace my_comp::domain::vram {

enum class TipoChave : std::uint8_t { Binario, WmClass, DesktopId };

class ChaveProcesso {
public:
    [[nodiscard]] static ChaveProcesso de_binario(QString valor);
    [[nodiscard]] static ChaveProcesso de_wm_class(QString valor);
    [[nodiscard]] static ChaveProcesso de_desktop_id(QString valor);

    [[nodiscard]] TipoChave tipo() const noexcept { return m_tipo; }
    [[nodiscard]] const QString& valor() const noexcept { return m_valor; }

    [[nodiscard]] bool operator==(const ChaveProcesso&) const noexcept = default;

private:
    ChaveProcesso(TipoChave tipo, QString valor);

    TipoChave m_tipo;
    QString m_valor;
};

}
