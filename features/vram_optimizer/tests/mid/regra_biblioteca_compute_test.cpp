#include "mid/classificacao/regras/lista_libs_compute.h"
#include "mid/classificacao/regras/regra_biblioteca_compute.h"
#include "mid/mocks/mock_leitor_maps_processo.h"
#include "back/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using vramopt::application::ListaLibsCompute;
using vramopt::application::RegraBibliotecaCompute;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockLeitorMapsProcesso;

namespace {

Processo processoPython() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("python")},
        BytesDeMemoria::deMegabytes(2048), 0, {}
    };
}

}

TEST(RegraBibliotecaComputeTest, ClassificaGpuQuandoLinkaCuda) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecasMapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libcuda.so.560")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    auto r = regra.avaliar(processoPython());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("biblioteca_compute"));
}

TEST(RegraBibliotecaComputeTest, JustificativaCitaSoEspecifica) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecasMapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libcudnn.so.9")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    auto r = regra.avaliar(processoPython());
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("libcudnn.so")));
}

TEST(RegraBibliotecaComputeTest, AbstemQuandoSoLibGL) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecasMapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libGL.so.1"),
                                     QStringLiteral("/usr/lib64/libvulkan.so.1")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    EXPECT_FALSE(regra.avaliar(processoPython()).has_value());
}

TEST(RegraBibliotecaComputeTest, AbstemQuandoSemLibs) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecasMapeadas(_)).WillOnce(Return(QStringList{}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    EXPECT_FALSE(regra.avaliar(processoPython()).has_value());
}
