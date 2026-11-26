#include "proc_stat.h"

#include "numeric_parse.h"

#include <fstream>

namespace my_comp::infrastructure::collectors {

namespace {
// Após o ')' o primeiro campo é o 3 (state); o starttime (22) é o 20º dali em diante.
constexpr int CAMPOS_ATE_STARTTIME = 22 - 3;
}  // namespace

std::optional<std::uint64_t> parse_stat_start_time(std::string_view linha) {
    const auto fecha = linha.find_last_of(')');
    if (fecha == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view resto = linha.substr(fecha + 1);
    for (int campo = 0;; ++campo) {
        const auto inicio = resto.find_first_not_of(' ');
        if (inicio == std::string_view::npos) {
            return std::nullopt;
        }
        resto = resto.substr(inicio);
        const auto fim = resto.find(' ');
        const std::string_view token = resto.substr(0, fim);
        if (campo == CAMPOS_ATE_STARTTIME) {
            return parse_integer<std::uint64_t>(token);
        }
        if (fim == std::string_view::npos) {
            return std::nullopt;
        }
        resto = resto.substr(fim);
    }
}

std::optional<std::uint64_t> read_start_time(int pid) {
    std::ifstream stat("/proc/" + std::to_string(pid) + "/stat");
    std::string linha;
    if (!std::getline(stat, linha)) {
        return std::nullopt;
    }
    return parse_stat_start_time(linha);
}

} // namespace my_comp::infrastructure::collectors
