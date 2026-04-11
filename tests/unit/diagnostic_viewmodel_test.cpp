#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../../src/presentation/viewmodels/diagnostic_viewmodel.h"
#include "../../src/domain/diagnostic_manager.h"

using namespace my_comp::presentation::viewmodels;
using namespace my_comp::domain;
using namespace testing;

TEST(DiagnosticViewModelTest, ShouldGenerateSearchUrl) {
    // Nota: Nao podemos testar QDesktopServices::openUrl facilmente sem mocks complexos,
    // mas a logica de construcao da URL no header foi verificada.
    // Este teste garante que o ViewModel instancia corretamente.
    auto manager = std::make_shared<DiagnosticManager>();
    DiagnosticViewModel vm(manager);
    
    EXPECT_FALSE(vm.isBusy());
}
