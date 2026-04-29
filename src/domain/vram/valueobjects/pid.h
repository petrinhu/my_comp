#pragma once

#include <cstdint>
#include <compare>

namespace vramopt::domain {

class PID {
public:
    using underlying_t = std::int32_t;

    explicit PID(underlying_t valor);

    [[nodiscard]] underlying_t valor() const noexcept { return m_valor; }

    [[nodiscard]] auto operator<=>(const PID&) const noexcept = default;
    [[nodiscard]] bool operator==(const PID&) const noexcept = default;

private:
    underlying_t m_valor;
};

}
