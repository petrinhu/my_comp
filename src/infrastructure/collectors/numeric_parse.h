#pragma once

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace my_comp::infrastructure::collectors {

// Converte texto de /proc ou /sys em inteiro sem lançar (MC-10, QA-13).
// Aceita espaços nas bordas; rejeita vazio, lixo após o número, estouro e
// sinal negativo em tipo sem sinal. std::stoi lançaria nesses casos.
template <typename T>
    requires std::is_integral_v<T>
[[nodiscard]] std::optional<T> parse_integer(std::string_view texto) noexcept {
    constexpr std::string_view espacos{" \t\r\n"};
    const auto inicio = texto.find_first_not_of(espacos);
    if (inicio == std::string_view::npos) {
        return std::nullopt;
    }
    const auto fim = texto.find_last_not_of(espacos);
    const std::string_view numero = texto.substr(inicio, fim - inicio + 1);

    T valor{};
    const auto [ptr, ec] = std::from_chars(numero.data(), numero.data() + numero.size(), valor);
    if (ec != std::errc{} || ptr != numero.data() + numero.size()) {
        return std::nullopt;
    }
    return valor;
}

} // namespace my_comp::infrastructure::collectors
