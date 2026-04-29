#include "back/valueobjects/pid.h"

#include <gtest/gtest.h>

#include <stdexcept>

using vramopt::domain::PID;

TEST(PIDTest, ConstroiComValorPositivo) {
    PID pid{1234};
    EXPECT_EQ(pid.valor(), 1234);
}

TEST(PIDTest, RejeitaZero) {
    EXPECT_THROW(PID{0}, std::invalid_argument);
}

TEST(PIDTest, RejeitaNegativo) {
    EXPECT_THROW(PID{-1}, std::invalid_argument);
}

TEST(PIDTest, IgualdadePorValor) {
    EXPECT_EQ(PID{42}, PID{42});
    EXPECT_NE(PID{42}, PID{43});
}

TEST(PIDTest, OrdenavelPorValor) {
    EXPECT_LT(PID{1}, PID{2});
}
