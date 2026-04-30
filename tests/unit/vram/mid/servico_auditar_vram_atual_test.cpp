#include "application/vram/auditoria/servico_auditar_vram_atual.h"
#include "application/vram/classificacao/servico_classificar_processo.h"
#include "application/vram/classificacao/servico_sugerir_acao.h"
#include "application/vram/configuracao/servico_localizar_config.h"
#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/classificacao/justificativa.h"
#include "domain/vram/classificacao/resultado_classificacao.h"
#include "domain/vram/configuracao/origem_config.h"
#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"
#include "mocks/mock_classificador_processos.h"
#include "mocks/mock_config_locator.h"
#include "mocks/mock_gpu_backend.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

using ::testing::_;
using ::testing::Return;

using vramopt::application::ServicoAuditarVramAtual;
using vramopt::application::ServicoClassificarProcesso;
using vramopt::application::ServicoLocalizarConfig;
using vramopt::application::ServicoSugerirAcao;
using vramopt::application::TipoSugestao;
using vramopt::domain::BytesDeMemoria;
using vramopt::domain::Categoria;
using vramopt::domain::CategoriaOrigem;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::Confianca;
using vramopt::domain::Fabricante;
using vramopt::domain::GpuInfo;
using vramopt::domain::Justificativa;
using vramopt::domain::NomeProcesso;
using vramopt::domain::OrigemConfig;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::domain::ResultadoClassificacao;
using vramopt::tests::MockClassificadorProcessos;
using vramopt::tests::MockConfigLocator;
using vramopt::tests::MockGpuBackend;

namespace {

ResultadoClassificacao classificacaoCpu() {
    return ResultadoClassificacao{
        .categoria = Categoria::CandidatoObvioCpu,
        .justificativa = Justificativa{
            .regra = QStringLiteral("catalogo"),
            .detalhe = QStringLiteral("casou binario=chrome"),
        },
    };
}

ResultadoClassificacao classificacaoGpu() {
    return ResultadoClassificacao{
        .categoria = Categoria::PrecisaGpu,
        .justificativa = Justificativa{
            .regra = QStringLiteral("comportamento"),
            .detalhe = QStringLiteral("compute_alto"),
        },
    };
}

OrigemConfig origemFalsa() {
    return OrigemConfig{
        .categoria = CategoriaOrigem::BrowserChromium,
        .tipo = QStringLiteral("browser-flag"),
        .caminho = QStringLiteral("/home/user/.config/google-chrome/Local State"),
        .chave = QStringLiteral("hardware_acceleration_mode.enabled"),
        .valor_atual = QStringLiteral("true"),
        .valor_sugerido = QStringLiteral("false"),
        .instrucao_humana = QStringLiteral("desligar aceleração"),
        .confianca = Confianca::Alta,
        .requer_reinicio_app = true,
        .aplicavel_automatico = false,
    };
}

}

TEST(ServicoAuditarVramAtualTest, AgrupaProcessosPorGpuEEnriqueceComTodosOsServicos) {
    auto backend = std::make_shared<MockGpuBackend>();
    auto classificador = std::make_shared<MockClassificadorProcessos>();
    auto locator = std::make_shared<MockConfigLocator>();

    GpuInfo gpu0{0, Fabricante::Nvidia, QStringLiteral("RTX 4070"),
                 BytesDeMemoria::deMegabytes(12288)};
    GpuInfo gpu1{1, Fabricante::Intel, QStringLiteral("Iris Xe"),
                 BytesDeMemoria::deMegabytes(2048)};

    Processo chrome{PID{1001},
                    NomeProcesso{QStringLiteral("chrome")},
                    BytesDeMemoria::deMegabytes(450), 0,
                    {ChaveProcesso::deBinario(QStringLiteral("chrome"))}};
    Processo blender{PID{2002},
                     NomeProcesso{QStringLiteral("blender")},
                     BytesDeMemoria::deMegabytes(3072), 1,
                     {ChaveProcesso::deBinario(QStringLiteral("blender"))}};

    EXPECT_CALL(*backend, enumerarGpus())
        .WillOnce(Return(std::vector<GpuInfo>{gpu0, gpu1}));
    EXPECT_CALL(*backend, enumerarProcessosVram(0))
        .WillOnce(Return(std::vector<Processo>{chrome}));
    EXPECT_CALL(*backend, enumerarProcessosVram(1))
        .WillOnce(Return(std::vector<Processo>{blender}));

    EXPECT_CALL(*classificador, classificar(_))
        .WillOnce(Return(classificacaoCpu()))
        .WillOnce(Return(classificacaoGpu()));

    EXPECT_CALL(*locator, localizar(_))
        .WillOnce(Return(QVector<OrigemConfig>{origemFalsa()}))
        .WillOnce(Return(QVector<OrigemConfig>{}));

    auto svc_class = std::make_shared<ServicoClassificarProcesso>(classificador);
    auto svc_sugerir = std::make_shared<ServicoSugerirAcao>();
    auto svc_localizar = std::make_shared<ServicoLocalizarConfig>(locator);

    ServicoAuditarVramAtual servico{backend, svc_class, svc_sugerir, svc_localizar};
    const auto relatorio = servico.executar();

    ASSERT_EQ(relatorio.grupos.size(), 2U);

    EXPECT_EQ(relatorio.grupos[0].gpu.id(), 0);
    EXPECT_EQ(relatorio.grupos[0].gpu.modelo(), QStringLiteral("RTX 4070"));
    ASSERT_EQ(relatorio.grupos[0].itens.size(), 1U);
    const auto& item_chrome = relatorio.grupos[0].itens[0];
    EXPECT_EQ(item_chrome.processo.pid().valor(), 1001);
    EXPECT_EQ(item_chrome.classificacao.categoria, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(item_chrome.sugestao.tipo, TipoSugestao::AjustarConfigDoApp);
    ASSERT_EQ(item_chrome.origens_config.size(), 1);
    EXPECT_EQ(item_chrome.origens_config.first().categoria,
              CategoriaOrigem::BrowserChromium);

    EXPECT_EQ(relatorio.grupos[1].gpu.id(), 1);
    ASSERT_EQ(relatorio.grupos[1].itens.size(), 1U);
    const auto& item_blender = relatorio.grupos[1].itens[0];
    EXPECT_EQ(item_blender.processo.pid().valor(), 2002);
    EXPECT_EQ(item_blender.classificacao.categoria, Categoria::PrecisaGpu);
    EXPECT_EQ(item_blender.sugestao.tipo, TipoSugestao::Nenhuma);
    EXPECT_TRUE(item_blender.origens_config.isEmpty());
}

TEST(ServicoAuditarVramAtualTest, RelatorioVazioQuandoNaoHaGpus) {
    auto backend = std::make_shared<MockGpuBackend>();
    auto classificador = std::make_shared<MockClassificadorProcessos>();
    auto locator = std::make_shared<MockConfigLocator>();

    EXPECT_CALL(*backend, enumerarGpus()).WillOnce(Return(std::vector<GpuInfo>{}));
    EXPECT_CALL(*backend, enumerarProcessosVram(_)).Times(0);
    EXPECT_CALL(*classificador, classificar(_)).Times(0);
    EXPECT_CALL(*locator, localizar(_)).Times(0);

    auto svc_class = std::make_shared<ServicoClassificarProcesso>(classificador);
    auto svc_sugerir = std::make_shared<ServicoSugerirAcao>();
    auto svc_localizar = std::make_shared<ServicoLocalizarConfig>(locator);

    ServicoAuditarVramAtual servico{backend, svc_class, svc_sugerir, svc_localizar};
    const auto relatorio = servico.executar();

    EXPECT_TRUE(relatorio.grupos.empty());
}

TEST(ServicoAuditarVramAtualTest, GpuSemProcessosFicaComItensVazios) {
    auto backend = std::make_shared<MockGpuBackend>();
    auto classificador = std::make_shared<MockClassificadorProcessos>();
    auto locator = std::make_shared<MockConfigLocator>();

    GpuInfo gpu{0, Fabricante::Amd, QStringLiteral("RX 7900"),
                BytesDeMemoria::deMegabytes(20480)};

    EXPECT_CALL(*backend, enumerarGpus()).WillOnce(Return(std::vector<GpuInfo>{gpu}));
    EXPECT_CALL(*backend, enumerarProcessosVram(0))
        .WillOnce(Return(std::vector<Processo>{}));
    EXPECT_CALL(*classificador, classificar(_)).Times(0);
    EXPECT_CALL(*locator, localizar(_)).Times(0);

    auto svc_class = std::make_shared<ServicoClassificarProcesso>(classificador);
    auto svc_sugerir = std::make_shared<ServicoSugerirAcao>();
    auto svc_localizar = std::make_shared<ServicoLocalizarConfig>(locator);

    ServicoAuditarVramAtual servico{backend, svc_class, svc_sugerir, svc_localizar};
    const auto relatorio = servico.executar();

    ASSERT_EQ(relatorio.grupos.size(), 1U);
    EXPECT_EQ(relatorio.grupos[0].gpu.id(), 0);
    EXPECT_TRUE(relatorio.grupos[0].itens.empty());
}
