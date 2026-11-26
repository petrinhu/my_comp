#pragma once

#include <QString>

namespace my_comp::domain::vram {

struct Justificativa {
    QString regra{};
    QString detalhe{};

    [[nodiscard]] bool operator==(const Justificativa&) const noexcept = default;
};

}
