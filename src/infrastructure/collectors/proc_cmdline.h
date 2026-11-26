#pragma once

#include <QString>

#include <string>

namespace my_comp::infrastructure::collectors {

// Formata o conteúdo cru de /proc/<pid>/cmdline (argumentos separados por NUL)
// como "argv0 arg1 arg2". Vazio (thread do kernel, zumbi) devolve fallback_comm.
// O comm de /proc/<pid>/stat é cortado em 15 caracteres (AUDIT-30).
[[nodiscard]] QString format_cmdline(const std::string& raw, const QString& fallback_comm);

} // namespace my_comp::infrastructure::collectors
