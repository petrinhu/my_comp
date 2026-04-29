#pragma once

#include <QString>

namespace vramopt::domain {

class NomeProcesso {
public:
    explicit NomeProcesso(QString valor);

    [[nodiscard]] const QString& valor() const noexcept { return m_valor; }

    [[nodiscard]] bool operator==(const NomeProcesso&) const noexcept = default;

private:
    QString m_valor;
};

}
