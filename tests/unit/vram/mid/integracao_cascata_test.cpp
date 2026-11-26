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

using vramopt::application::ClassificadorEmCascata;
using vramopt::application::ListaLibsCompute;
using vramopt::application::MapaCategoriasDesktop;
using vramopt::application::RegraBibliotecaCompute;
using vramopt::application::RegraCatalogo;
using vramopt::application::RegraCategoriaDesktop;
using vramopt::application::RegraComportamento;
using vramopt::application::RegraFeedback;
using vramopt::domain::AmostraJanela;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::EntradaCatalogo;
using vramopt::domain::IRegraClassificacao;
using vramopt::domain::LimiaresComportamento;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::tests::MockAmostradorComportamento;
using vramopt::tests::MockCatalogoClassificacao;
using vramopt::tests::MockLeitorDesktopFile;
using vramopt::tests::MockLeitorMapsProcesso;
using vramopt::tests::MockRepositorioFeedback;

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
        .WillOnce(Return(vramopt::domain::EntradaFeedback{
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
