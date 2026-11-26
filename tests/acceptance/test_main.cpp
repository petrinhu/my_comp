#include <gtest/gtest.h>
#include <QCoreApplication>

// main dos testes de aceitação: um QCoreApplication para todos os cenários (QSignalSpy,
// QtConcurrent + invokeMethod precisam de event loop). Testes que dependem de hardware ou
// ferramenta ausente usam GTEST_SKIP.
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
