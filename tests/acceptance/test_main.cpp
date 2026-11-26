#include <gtest/gtest.h>

// main dos testes de aceitação: cada teste que precisa de event loop cria o próprio
// QCoreApplication. Testes que dependem de hardware/ferramenta ausente usam GTEST_SKIP.
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
