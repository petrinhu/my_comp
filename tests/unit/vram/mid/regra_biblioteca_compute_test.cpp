#include "application/vram/classificacao/regras/lista_libs_compute.h"
#include "application/vram/classificacao/regras/regra_biblioteca_compute.h"
#include "mocks/mock_leitor_maps_processo.h"
#include "domain/vram/entidades/processo.h"

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

Processo processo_python() {
    return Processo{
        PID{4321}, NomeProcesso{QStringLiteral("python")},
        BytesDeMemoria::de_megabytes(2048), 0, {}
    };
}

}

TEST(RegraBibliotecaComputeTest, ClassificaGpuQuandoLinkaCuda) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecas_mapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libcuda.so.560")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    auto r = regra.avaliar(processo_python());
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(r->justificativa.regra, QStringLiteral("biblioteca_compute"));
}

TEST(RegraBibliotecaComputeTest, JustificativaCitaSoEspecifica) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecas_mapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libcudnn.so.9")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    auto r = regra.avaliar(processo_python());
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->justificativa.detalhe.contains(QStringLiteral("libcudnn.so")));
}

TEST(RegraBibliotecaComputeTest, AbstemQuandoSoLibGL) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecas_mapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libGL.so.1"),
                                     QStringLiteral("/usr/lib64/libvulkan.so.1")}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    EXPECT_FALSE(regra.avaliar(processo_python()).has_value());
}

TEST(RegraBibliotecaComputeTest, AbstemQuandoSemLibs) {
    auto leitor = std::make_shared<MockLeitorMapsProcesso>();
    EXPECT_CALL(*leitor, bibliotecas_mapeadas(_)).WillOnce(Return(QStringList{}));
    RegraBibliotecaCompute regra{leitor, ListaLibsCompute::padrao()};
    EXPECT_FALSE(regra.avaliar(processo_python()).has_value());
}
