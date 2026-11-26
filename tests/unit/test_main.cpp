#include <gtest/gtest.h>
#include <QCoreApplication>

// main dos testes unitários: QCoreApplication é necessário para QSignalSpy, QSettings
// e invokeMethod(QueuedConnection) usados pelos testes de ViewModel.
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
