#include "domain/vram/interfaces/i_amostrador_comportamento.h"
#include "domain/vram/interfaces/i_catalogo_classificacao.h"
#include "domain/vram/interfaces/i_classificador_processos.h"
#include "domain/vram/interfaces/i_leitor_desktop_file.h"
#include "domain/vram/interfaces/i_leitor_maps_processo.h"
#include "domain/vram/interfaces/i_regra_classificacao.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <gtest/gtest.h>

#include <type_traits>

TEST(IClassificadorProcessosTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IClassificadorProcessos>);
    SUCCEED();
}

TEST(IRegraClassificacaoTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IRegraClassificacao>);
    SUCCEED();
}

TEST(ICatalogoClassificacaoTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ICatalogoClassificacao>);
    SUCCEED();
}

TEST(IRepositorioFeedbackTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IRepositorioFeedback>);
    SUCCEED();
}

TEST(ILeitorDesktopFileTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ILeitorDesktopFile>);
    SUCCEED();
}

TEST(ILeitorMapsProcessoTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ILeitorMapsProcesso>);
    SUCCEED();
}

TEST(IAmostradorComportamentoTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IAmostradorComportamento>);
    SUCCEED();
}

#include "domain/vram/interfaces/i_config_locator.h"
#include "domain/vram/interfaces/i_gpu_backend.h"
#include "domain/vram/interfaces/i_leitor_arquivo_config.h"
#include "domain/vram/interfaces/i_leitor_desktop_exec.h"
#include "domain/vram/interfaces/i_leitor_environ.h"

TEST(IConfigLocatorTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IConfigLocator>);
    SUCCEED();
}
TEST(ILeitorEnvironTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ILeitorEnviron>);
    SUCCEED();
}
TEST(ILeitorArquivoConfigTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ILeitorArquivoConfig>);
    SUCCEED();
}
TEST(ILeitorDesktopExecTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::ILeitorDesktopExec>);
    SUCCEED();
}
TEST(IGpuBackendTest, EAbstrata) {
    static_assert(std::is_abstract_v<my_comp::domain::vram::IGpuBackend>);
    SUCCEED();
}
