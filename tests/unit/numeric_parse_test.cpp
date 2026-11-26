#include <gtest/gtest.h>

#include "infrastructure/collectors/numeric_parse.h"

#include <cstdint>

using my_comp::infrastructure::collectors::parse_integer;

// /proc e /sys são entrada externa (MC-10): conversões não podem lançar (QA-13).

TEST(NumericParseTest, ConverteInteiroValido) {
    EXPECT_EQ(parse_integer<int>("42"), 42);
    EXPECT_EQ(parse_integer<std::uint64_t>("16384256"), 16384256U);
}

TEST(NumericParseTest, IgnoraEspacosNasBordas) {
    EXPECT_EQ(parse_integer<int>("  8 \n"), 8);
}

TEST(NumericParseTest, RejeitaTextoVazioOuNaoNumerico) {
    EXPECT_FALSE(parse_integer<int>("").has_value());
    EXPECT_FALSE(parse_integer<int>("   ").has_value());
    EXPECT_FALSE(parse_integer<int>("abc").has_value());
    EXPECT_FALSE(parse_integer<int>("12abc").has_value());
}

TEST(NumericParseTest, RejeitaEstouroSemLancar) {
    EXPECT_FALSE(parse_integer<int>("99999999999999999999").has_value());
    EXPECT_FALSE(parse_integer<std::uint64_t>("999999999999999999999999").has_value());
}

TEST(NumericParseTest, RejeitaNegativoEmTipoSemSinal) {
    EXPECT_FALSE(parse_integer<std::uint64_t>("-1").has_value());
}
