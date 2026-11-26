#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace my_comp::infrastructure::collectors {

// Extrai o campo 22 (starttime, ticks desde o boot) de uma linha de /proc/<pid>/stat.
// O comm (campo 2) pode ter espaços e ')'; a contagem começa após o último ')'.
// PID + starttime identificam um processo mesmo com o PID reciclado.
[[nodiscard]] std::optional<std::uint64_t> parse_stat_start_time(std::string_view linha);

// Lê /proc/<pid>/stat e devolve o starttime; vazio se o processo não existe mais.
[[nodiscard]] std::optional<std::uint64_t> read_start_time(int pid);

} // namespace my_comp::infrastructure::collectors
