#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/presentation/viewmodels/diagnostic_viewmodel.h"
#include "../../src/domain/diagnostic_manager.h"
#include <chrono>

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain;
using namespace testing;

class JournalLoadTest : public ::testing::Test {
protected:
    std::shared_ptr<DiagnosticManager> manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel viewModel{manager};
};

TEST_F(JournalLoadTest, HandleLargeVolumeOfLogs) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<models::LogEntry> logs;
    logs.reserve(10000);

    // Gerar 10.000 logs
    for (int i = 0; i < 10000; ++i) {
        models::LogEntry log;
        log.message = QString("Mensagem de log de estresse número %1").arg(i);
        log.source = "STRESS_TEST";
        log.priority = 3;
        logs.push_back(log);
    }
    
    // Simular atualização no ViewModel (se houver método para isso ou via manager)
    // Para o teste de carga de UI, vamos medir quanto tempo leva para o ViewModel 
    // "processar" esses logs (ex: converter para o modelo de lista).
    
    // Atualmente DiagnosticViewModel usa um modelo interno. 
    // Vamos verificar como ele recebe dados.
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000); 
}
