#include "composition/vram_composition.h"

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"
#include "mocks/mock_gpu_backend.h"

#include <gmock/gmock.h>

#include <chrono>

#include <gtest/gtest.h>

// A composição única (usada por GUI e CLI) monta a cascata completa; antes só a
// RegraCategoriaDesktop era registrada e as outras 4 regras eram código morto.
TEST(ComposicaoVramTest, CascataRegistraAsCincoRegrasNaOrdemDocumentada) {
    const auto nomes = my_comp::composition::nomes_das_regras_da_cascata();
    const std::vector<QString> esperado = {
        QStringLiteral("feedback"), QStringLiteral("catalogo"), QStringLiteral("categoria_desktop"),
        QStringLiteral("biblioteca_compute"), QStringLiteral("comportamento"),
    };
    EXPECT_EQ(nomes, esperado);
}

TEST(ComposicaoVramTest, ComporServicoDevolveInstanciaUtilizavel) {
    auto servico = my_comp::composition::compor_servico_auditar_vram();
    ASSERT_NE(servico, nullptr);
    (void)servico->executar(); // sem GPU no ambiente de teste: relatório vazio, sem crash
}

// VRAM-06: a RegraCatalogo da composição consulta o catálogo empacotado, não o null object.
TEST(ComposicaoVramTest, CatalogoDaComposicaoEOEmpacotado) {
    const auto catalogo = my_comp::composition::compor_catalogo_classificacao();
    ASSERT_NE(catalogo, nullptr);
    const auto firefox = catalogo->buscar(
        my_comp::domain::vram::ChaveProcesso::de_binario(QStringLiteral("firefox")));
    ASSERT_TRUE(firefox.has_value());
    EXPECT_EQ(firefox->veredito, my_comp::domain::vram::Categoria::CandidatoObvioCpu);
}

// VRAM-07: a RegraFeedback da composição lê o repositório cifrado, não o null object.
TEST(ComposicaoVramTest, RepositorioDeFeedbackDaComposicaoECifrado) {
    const auto repo = my_comp::composition::compor_repositorio_feedback();
    ASSERT_NE(repo, nullptr);
    EXPECT_NE(dynamic_cast<const my_comp::infrastructure::vram::RepositorioFeedbackCifrado*>(repo.get()),
              nullptr);
}

TEST(ComposicaoVramTest, ServicosVramIncluemRegistroDeFeedback) {
    const auto servicos = my_comp::composition::compor_servicos_vram();
    EXPECT_NE(servicos.auditar, nullptr);
    EXPECT_NE(servicos.registrar_feedback, nullptr);
}

// VRAM-08: o backend usado pela auditoria alimenta o amostrador da RegraComportamento.
TEST(ComposicaoVramTest, BackendDaAuditoriaAlimentaOAmostrador) {
    using my_comp::domain::vram::BytesDeMemoria;
    using my_comp::domain::vram::ChaveProcesso;
    using my_comp::domain::vram::Fabricante;
    using my_comp::domain::vram::GpuInfo;
    using my_comp::domain::vram::NomeProcesso;
    using my_comp::domain::vram::PID;
    using my_comp::domain::vram::Processo;
    using ::testing::Return;

    auto real = std::make_shared<my_comp::tests::vram::MockGpuBackend>();
    EXPECT_CALL(*real, enumerar_gpus()).WillOnce(Return(std::vector<GpuInfo>{
        GpuInfo{0, Fabricante::Intel, QStringLiteral("Arc"), BytesDeMemoria::de_megabytes(4096)}}));
    EXPECT_CALL(*real, enumerar_processos_vram(0)).WillOnce(Return(std::vector<Processo>{
        Processo{PID{77}, NomeProcesso{QStringLiteral("foo")}, BytesDeMemoria::de_megabytes(40), 0,
                 {ChaveProcesso::de_binario(QStringLiteral("foo"))}}}));

    const auto montado = my_comp::composition::compor_backend_com_amostrador(real);
    ASSERT_NE(montado.backend, nullptr);
    ASSERT_NE(montado.amostrador, nullptr);
    (void)montado.backend->enumerar_gpus();
    (void)montado.backend->enumerar_processos_vram(0);

    const auto a = montado.amostrador->amostra_janela(PID{77}, std::chrono::seconds{30});
    EXPECT_EQ(a.amostras, 1);
    EXPECT_EQ(a.vram_media, BytesDeMemoria::de_megabytes(40));
}
