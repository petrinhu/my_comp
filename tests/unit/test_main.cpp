#include <gtest/gtest.h>
#include <QCoreApplication>

TEST(PlaceholderTest, InitialSetup) {
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
