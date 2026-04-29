#pragma once

#include <cstdint>
#include <compare>

namespace vramopt::domain {

class BytesDeMemoria {
public:
    using underlying_t = std::uint64_t;

    explicit BytesDeMemoria(underlying_t valor) noexcept : m_valor{valor} {}

    [[nodiscard]] static BytesDeMemoria deMegabytes(underlying_t mb) noexcept;
    [[nodiscard]] static BytesDeMemoria deKilobytes(underlying_t kb) noexcept;

    [[nodiscard]] underlying_t valor() const noexcept { return m_valor; }
    [[nodiscard]] double razaoSobre(BytesDeMemoria total) const noexcept;

    [[nodiscard]] auto operator<=>(const BytesDeMemoria&) const noexcept = default;
    [[nodiscard]] bool operator==(const BytesDeMemoria&) const noexcept = default;

private:
    underlying_t m_valor;
};

}
