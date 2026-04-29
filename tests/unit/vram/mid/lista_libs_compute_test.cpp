#include "application/vram/classificacao/regras/lista_libs_compute.h"

#include <gtest/gtest.h>

using vramopt::application::ListaLibsCompute;

TEST(ListaLibsComputeTest, PadraoMatchaCuda) {
    auto lista = ListaLibsCompute::padrao();
    auto m = lista.primeiraQueCasa({QStringLiteral("/usr/lib64/libcuda.so.560.35.03")});
    ASSERT_TRUE(m.has_value());
    EXPECT_TRUE(m->contains(QStringLiteral("libcuda.so")));
}

TEST(ListaLibsComputeTest, PadraoMatchaCudnn) {
    auto lista = ListaLibsCompute::padrao();
    auto m = lista.primeiraQueCasa({QStringLiteral("/path/libcudnn.so.9")});
    ASSERT_TRUE(m.has_value());
}

TEST(ListaLibsComputeTest, PadraoMatchaLevelZero) {
    auto lista = ListaLibsCompute::padrao();
    auto m = lista.primeiraQueCasa({QStringLiteral("/usr/lib64/libze_loader.so.1")});
    ASSERT_TRUE(m.has_value());
}

TEST(ListaLibsComputeTest, PadraoMatchaOpenCL) {
    auto lista = ListaLibsCompute::padrao();
    auto m = lista.primeiraQueCasa({QStringLiteral("/usr/lib64/libOpenCL.so.1")});
    ASSERT_TRUE(m.has_value());
}

TEST(ListaLibsComputeTest, PadraoMatchaRocm) {
    auto lista = ListaLibsCompute::padrao();
    auto m = lista.primeiraQueCasa({QStringLiteral("/opt/rocm/lib/libamdhip64.so.6")});
    ASSERT_TRUE(m.has_value());
}

TEST(ListaLibsComputeTest, NaoMatchaLibGL) {
    auto lista = ListaLibsCompute::padrao();
    EXPECT_FALSE(lista.primeiraQueCasa(
        {QStringLiteral("/usr/lib64/libGL.so.1"),
         QStringLiteral("/usr/lib64/libGLX.so.0"),
         QStringLiteral("/usr/lib64/libEGL.so.1"),
         QStringLiteral("/usr/lib64/libvulkan.so.1"),
         QStringLiteral("/usr/lib64/libVA.so.2")}).has_value());
}

TEST(ListaLibsComputeTest, ListaVaziaAbstem) {
    auto lista = ListaLibsCompute::padrao();
    EXPECT_FALSE(lista.primeiraQueCasa({}).has_value());
}
