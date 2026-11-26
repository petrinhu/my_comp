#include "application/vram/classificacao/classificador_em_cascata.h"
#include "application/vram/classificacao/regras/lista_libs_compute.h"
#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"
#include "application/vram/classificacao/regras/regra_biblioteca_compute.h"
#include "application/vram/classificacao/regras/regra_catalogo.h"
#include "application/vram/classificacao/regras/regra_categoria_desktop.h"
#include "application/vram/classificacao/regras/regra_comportamento.h"
#include "application/vram/classificacao/regras/regra_feedback.h"
#include "mocks/mock_amostrador_comportamento.h"
#include "mocks/mock_catalogo_classificacao.h"
#include "mocks/mock_leitor_desktop_file.h"
#include "mocks/mock_leitor_maps_processo.h"
#include "mocks/mock_repositorio_feedback.h"
#include "domain/vram/entidades/processo.h"

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::_;
using ::testing::Return;

using my_comp::application::vram::ClassificadorEmCascata;
using my_comp::application::vram::ListaLibsCompute;
using my_comp::application::vram::MapaCategoriasDesktop;
using my_comp::application::vram::RegraBibliotecaCompute;
using my_comp::application::vram::RegraCatalogo;
using my_comp::application::vram::RegraCategoriaDesktop;
using my_comp::application::vram::RegraComportamento;
using my_comp::application::vram::RegraFeedback;
using my_comp::domain::vram::AmostraJanela;
using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::EntradaCatalogo;
using my_comp::domain::vram::IRegraClassificacao;
using my_comp::domain::vram::LimiaresComportamento;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::tests::vram::MockAmostradorComportamento;
using my_comp::tests::vram::MockCatalogoClassificacao;
using my_comp::tests::vram::MockLeitorDesktopFile;
using my_comp::tests::vram::MockLeitorMapsProcesso;
using my_comp::tests::vram::MockRepositorioFeedback;

namespace {

struct CenarioMontado {
    std::shared_ptr<MockRepositorioFeedback>      repo;
    std::shared_ptr<MockCatalogoClassificacao>    catalogo;
    std::shared_ptr<MockLeitorDesktopFile>        leitor_desktop;
    std::shared_ptr<MockLeitorMapsProcesso>       leitor_maps;
    std::shared_ptr<MockAmostradorComportamento>  amostrador;
    ClassificadorEmCascata                        cascata;
};

CenarioMontado montar() {
    auto repo            = std::make_shared<MockRepositorioFeedback>();
    auto catalogo        = std::make_shared<MockCatalogoClassificacao>();
    auto leitor_desktop  = std::make_shared<MockLeitorDesktopFile>();
    auto leitor_maps     = std::make_shared<MockLeitorMapsProcesso>();
    auto amostrador      = std::make_shared<MockAmostradorComportamento>();

    std::vector<std::shared_ptr<IRegraClassificacao>> regras = {
        std::make_shared<RegraFeedback>(repo),
        std::make_shared<RegraCatalogo>(catalogo),
        std::make_shared<RegraCategoriaDesktop>(leitor_desktop, MapaCategoriasDesktop::padrao()),
        std::make_shared<RegraBibliotecaCompute>(leitor_maps, ListaLibsCompute::padrao()),
        std::make_shared<RegraComportamento>(amostrador, LimiaresComportamento::padrao(),
                                             std::chrono::seconds{30}),
    };
    return CenarioMontado{repo, catalogo, leitor_desktop, leitor_maps, amostrador,
                          ClassificadorEmCascata{std::move(regras)}};
}

Processo proc_firefox_idle() {
    return Processo{
        PID{1234}, NomeProcesso{QStringLiteral("firefox")},
        BytesDeMemoria::de_megabytes(350), 0,
        {ChaveProcesso::de_binario(QStringLiteral("firefox"))}
    };
}

}

TEST(IntegracaoCascataTest, FirefoxIdleClassificadoPeloCatalogo) {
    auto c = montar();

    EXPECT_CALL(*c.repo, buscar(_)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*c.catalogo, buscar(_)).WillOnce(Return(EntradaCatalogo{
        .id = QStringLiteral("firefox"),
        .binarios = {QStringLiteral("firefox")},
        .wm_class = {QStringLiteral("firefox")},
        .desktop_id = {QStringLiteral("firefox.desktop")},
        .veredito = Categoria::CandidatoObvioCpu,
        .justificativa = QStringLiteral("Navegador."),
    }));
    EXPECT_CALL(*c.leitor_desktop, categorias_de(_)).Times(0);
    EXPECT_CALL(*c.leitor_maps, bibliotecas_mapeadas(_)).Times(0);
    EXPECT_CALL(*c.amostrador, amostra_janela(_, _)).Times(0);

    auto v = c.cascata.classificar(proc_firefox_idle());
    EXPECT_EQ(v.categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("catalogo"));
}

TEST(IntegracaoCascataTest, PythonComCudaClassificadoPelaBibliotecaCompute) {
    auto c = montar();

    EXPECT_CALL(*c.repo, buscar(_)).WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*c.catalogo, buscar(_)).WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*c.leitor_desktop, categorias_de(_)).WillOnce(Return(QStringList{}));
    EXPECT_CALL(*c.leitor_maps, bibliotecas_mapeadas(_))
        .WillOnce(Return(QStringList{QStringLiteral("/usr/lib64/libcudart.so.12")}));
    EXPECT_CALL(*c.amostrador, amostra_janela(_, _)).Times(0);

    Processo python{PID{4321}, NomeProcesso{QStringLiteral("python")},
                    BytesDeMemoria::de_megabytes(2000), 0,
                    {ChaveProcesso::de_binario(QStringLiteral("python"))}};
    auto v = c.cascata.classificar(python);
    EXPECT_EQ(v.categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("biblioteca_compute"));
}

TEST(IntegracaoCascataTest, AppDesconhecidoLeveCaiEmComportamento) {
    auto c = montar();

    EXPECT_CALL(*c.repo, buscar(_)).WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*c.catalogo, buscar(_)).WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*c.leitor_desktop, categorias_de(_)).WillOnce(Return(QStringList{}));
    EXPECT_CALL(*c.leitor_maps, bibliotecas_mapeadas(_)).WillOnce(Return(QStringList{}));
    EXPECT_CALL(*c.amostrador, amostra_janela(_, _))
        .WillOnce(Return(AmostraJanela{
            .vram_media = BytesDeMemoria::de_megabytes(50),
            .vram_total_gpu = BytesDeMemoria::de_megabytes(8192),
            .compute_utilizacao_media = 0.01,
            .amostras = 30,
            .janela = std::chrono::seconds{30},
        }));

    Processo desc{PID{99}, NomeProcesso{QStringLiteral("foo")},
                  BytesDeMemoria::de_megabytes(50), 0,
                  {ChaveProcesso::de_binario(QStringLiteral("foo"))}};
    auto v = c.cascata.classificar(desc);
    EXPECT_EQ(v.categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("comportamento"));
}

TEST(IntegracaoCascataTest, FeedbackVenceCatalogoMesmoQueCatalogoTambemDecidiria) {
    auto c = montar();

    EXPECT_CALL(*c.repo, buscar(_))
        .WillOnce(Return(my_comp::domain::vram::EntradaFeedback{
            .id = 7,
            .chave = ChaveProcesso::de_binario(QStringLiteral("firefox")),
            .veredito = Categoria::PrecisaGpu,
            .motivo = QStringLiteral("uso WebGL diariamente"),
            .criado_em = QDateTime::currentDateTimeUtc(),
            .visto_em = QDateTime::currentDateTimeUtc(),
            .revisado_em = std::nullopt,
        }));
    EXPECT_CALL(*c.catalogo, buscar(_)).Times(0);

    auto v = c.cascata.classificar(proc_firefox_idle());
    EXPECT_EQ(v.categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(v.justificativa.regra, QStringLiteral("feedback"));
}
