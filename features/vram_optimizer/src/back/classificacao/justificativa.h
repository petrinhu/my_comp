#pragma once

#include <QString>

namespace vramopt::domain {

struct Justificativa {
    QString regra{};
    QString detalhe{};

    [[nodiscard]] bool operator==(const Justificativa&) const noexcept = default;
};

}
