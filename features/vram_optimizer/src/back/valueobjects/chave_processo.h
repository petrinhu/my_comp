#pragma once

#include <cstdint>

#include <QString>

namespace vramopt::domain {

enum class TipoChave : std::uint8_t { Binario, WmClass, DesktopId };

class ChaveProcesso {
public:
    [[nodiscard]] static ChaveProcesso deBinario(QString valor);
    [[nodiscard]] static ChaveProcesso deWmClass(QString valor);
    [[nodiscard]] static ChaveProcesso deDesktopId(QString valor);

    [[nodiscard]] TipoChave tipo() const noexcept { return m_tipo; }
    [[nodiscard]] const QString& valor() const noexcept { return m_valor; }

    [[nodiscard]] bool operator==(const ChaveProcesso&) const noexcept = default;

private:
    ChaveProcesso(TipoChave tipo, QString valor);

    TipoChave m_tipo;
    QString m_valor;
};

}
