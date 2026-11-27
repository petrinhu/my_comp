#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QThreadPool>

namespace {

// Ao fim de cada teste, espera os workers do QtConcurrent e entrega os eventos que eles
// postaram. Assim nenhum worker sobrevive ao teste que o criou e escreve em variável já
// fora de escopo (revisão do UX-05), mesmo quando um ASSERT aborta o teste no meio.
class EsperaWorkersAoFimDoTeste : public ::testing::EmptyTestEventListener {
    void OnTestEnd(const ::testing::TestInfo& /*info*/) override {
        QThreadPool::globalInstance()->waitForDone();
        QCoreApplication::processEvents();
    }
};

}  // namespace

// main dos testes unitários: QCoreApplication é necessário para QSignalSpy, QSettings
// e invokeMethod(QueuedConnection) usados pelos testes de ViewModel.
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new EsperaWorkersAoFimDoTeste);
    return RUN_ALL_TESTS();
}
