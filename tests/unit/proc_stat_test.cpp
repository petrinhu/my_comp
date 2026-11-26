#include <gtest/gtest.h>

#include "infrastructure/collectors/proc_stat.h"

#include <string>

using my_comp::infrastructure::collectors::parse_stat_start_time;

// Campo 22 de /proc/<pid>/stat (starttime, em ticks desde o boot): identifica o
// processo junto com o PID, que o kernel recicla.

namespace {
// Monta uma linha de stat com `comm` e starttime nos lugares certos (campos 3..22).
std::string linha_stat(const std::string& comm, const std::string& starttime) {
    return "4242 (" + comm + ") S 1 4242 4242 0 -1 4194560 100 0 0 0 5 3 0 0 20 0 1 0 "
         + starttime + " 12345678 300 18446744073709551615 1 1 0 0 0 0 0 0 0 0 0 0 17 3 0 0 0 0 0";
}
}  // namespace

TEST(ProcStatTest, LeStarttime) {
    EXPECT_EQ(parse_stat_start_time(linha_stat("bash", "987654")), 987654U);
}

TEST(ProcStatTest, CommComEspacoEParentesesNaoDesalinhaOsCampos) {
    EXPECT_EQ(parse_stat_start_time(linha_stat("a) b (c) d", "555")), 555U);
}

TEST(ProcStatTest, LinhaTruncadaOuSemParentesesDevolveVazio) {
    EXPECT_FALSE(parse_stat_start_time("4242 (bash) S 1 2 3").has_value());
    EXPECT_FALSE(parse_stat_start_time("4242 bash S 1").has_value());
    EXPECT_FALSE(parse_stat_start_time("").has_value());
}

TEST(ProcStatTest, StarttimeNaoNumericoDevolveVazio) {
    EXPECT_FALSE(parse_stat_start_time(linha_stat("bash", "xyz")).has_value());
}
