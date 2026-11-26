#pragma once

#include <QString>

#include <cstddef>
#include <string>

namespace my_comp::infrastructure::collectors {

// Formata o conteúdo cru de /proc/<pid>/cmdline (argumentos separados por NUL)
// como "argv0 arg1 arg2". Vazio (thread do kernel, zumbi) devolve fallback_comm.
// O comm de /proc/<pid>/stat é cortado em 15 caracteres (AUDIT-30).
[[nodiscard]] QString format_cmdline(const std::string& raw, const QString& fallback_comm);

// Teto de leitura do cmdline: uma JVM pode ter vários MB de classpath, lidos a cada
// atualização da lista de processos. O excedente é descartado.
inline constexpr std::size_t MAX_CMDLINE_BYTES = 4096;

// Lê até MAX_CMDLINE_BYTES de `path` (/proc/<pid>/cmdline) e formata com format_cmdline.
// Arquivo ausente ou ilegível (processo já encerrado) devolve fallback_comm.
[[nodiscard]] QString read_cmdline(const QString& path, const QString& fallback_comm);

} // namespace my_comp::infrastructure::collectors
