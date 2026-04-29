#include "back/interfaces/i_amostrador_comportamento.h"
#include "back/interfaces/i_catalogo_classificacao.h"
#include "back/interfaces/i_classificador_processos.h"
#include "back/interfaces/i_leitor_desktop_file.h"
#include "back/interfaces/i_leitor_maps_processo.h"
#include "back/interfaces/i_regra_classificacao.h"
#include "back/interfaces/i_repositorio_feedback.h"

#include <gtest/gtest.h>

#include <type_traits>

TEST(IClassificadorProcessosTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::IClassificadorProcessos>);
    SUCCEED();
}

TEST(IRegraClassificacaoTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::IRegraClassificacao>);
    SUCCEED();
}

TEST(ICatalogoClassificacaoTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ICatalogoClassificacao>);
    SUCCEED();
}

TEST(IRepositorioFeedbackTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::IRepositorioFeedback>);
    SUCCEED();
}

TEST(ILeitorDesktopFileTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ILeitorDesktopFile>);
    SUCCEED();
}

TEST(ILeitorMapsProcessoTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ILeitorMapsProcesso>);
    SUCCEED();
}

TEST(IAmostradorComportamentoTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::IAmostradorComportamento>);
    SUCCEED();
}

#include "back/interfaces/i_config_locator.h"
#include "back/interfaces/i_leitor_arquivo_config.h"
#include "back/interfaces/i_leitor_desktop_exec.h"
#include "back/interfaces/i_leitor_environ.h"

TEST(IConfigLocatorTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::IConfigLocator>);
    SUCCEED();
}
TEST(ILeitorEnvironTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ILeitorEnviron>);
    SUCCEED();
}
TEST(ILeitorArquivoConfigTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ILeitorArquivoConfig>);
    SUCCEED();
}
TEST(ILeitorDesktopExecTest, EAbstrata) {
    static_assert(std::is_abstract_v<vramopt::domain::ILeitorDesktopExec>);
    SUCCEED();
}
