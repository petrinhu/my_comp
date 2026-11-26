#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/auditoria/servico_auditar_vram_atual.h"
#include "application/vram/classificacao/sugestao_acao.h"
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
#include "presentation/cli/cli_vram.h"
#include "mocks/mock_classificador_processos.h"
#include "mocks/mock_config_locator.h"
#include "mocks/mock_gpu_backend.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::Return;

using vramopt::application::GrupoAuditoriaGpu;
using vramopt::application::ItemAuditoriaVram;
using vramopt::application::RelatorioAuditoriaVram;
using vramopt::application::ServicoAuditarVramAtual;
using vramopt::application::ServicoClassificarProcesso;
using vramopt::application::ServicoLocalizarConfig;
using vramopt::application::ServicoSugerirAcao;
using vramopt::application::SugestaoAcao;
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
using vramopt::presentation::cli::executar_com_servico;
using vramopt::presentation::cli::formatar_relatorio;
using vramopt::presentation::cli::OpcoesCliVram;
using vramopt::presentation::cli::parsear_opcoes;
using vramopt::tests::MockClassificadorProcessos;
using vramopt::tests::MockConfigLocator;
using vramopt::tests::MockGpuBackend;

namespace {

RelatorioAuditoriaVram fazer_relatorio() {
    RelatorioAuditoriaVram r;
    GrupoAuditoriaGpu grupo{
        .gpu = GpuInfo{0, Fabricante::Nvidia, QStringLiteral("RTX 4070"),
                       BytesDeMemoria::de_megabytes(12288)},
        .itens = {},
    };
    grupo.itens.push_back(ItemAuditoriaVram{
        .processo = Processo{PID{1234},
                             NomeProcesso{QStringLiteral("blender")},
                             BytesDeMemoria::de_megabytes(2048),
                             0,
                             {ChaveProcesso::de_binario(QStringLiteral("blender"))}},
        .classificacao = ResultadoClassificacao{
            .categoria = Categoria::PrecisaGpu,
            .justificativa = Justificativa{.regra = QStringLiteral("comportamento"), .detalhe = {}},
        },
        .sugestao = SugestaoAcao{.tipo = TipoSugestao::Nenhuma, .descricao = {}},
        .origens_config = {},
    });
    r.grupos.push_back(std::move(grupo));
    return r;
}

}

TEST(CliVramOpcoesTest, JsonAtivaModoJson) {
    auto opts = parsear_opcoes({QStringLiteral("--json")});
    EXPECT_TRUE(opts.json);
    EXPECT_TRUE(opts.include_config_origins);
}

TEST(CliVramOpcoesTest, AllProcessesAtivaFlagCorrespondente) {
    auto opts = parsear_opcoes({QStringLiteral("--all-processes")});
    EXPECT_TRUE(opts.all_processes);
    EXPECT_FALSE(opts.json);
}

TEST(CliVramOpcoesTest, NoConfigOriginsDesligaInclude) {
    auto opts = parsear_opcoes({QStringLiteral("--no-config-origins")});
    EXPECT_FALSE(opts.include_config_origins);
}

TEST(CliVramFormatTest, ModoJsonProduzEstruturaValida) {
    OpcoesCliVram opts; opts.json = true;
    const auto saida = formatar_relatorio(fazer_relatorio(), opts);

    QJsonParseError err{};
    auto doc = QJsonDocument::fromJson(saida.toUtf8(), &err);
    ASSERT_EQ(err.error, QJsonParseError::NoError) << err.errorString().toStdString();
    ASSERT_TRUE(doc.isObject());

    const auto root = doc.object();
    const auto gpus = root.value(QStringLiteral("gpus")).toArray();
    ASSERT_EQ(gpus.size(), 1);
    const auto g0 = gpus.at(0).toObject();
    EXPECT_EQ(g0.value(QStringLiteral("gpu_id")).toInt(), 0);
    EXPECT_EQ(g0.value(QStringLiteral("modelo")).toString(), QStringLiteral("RTX 4070"));
    const auto itens = g0.value(QStringLiteral("itens")).toArray();
    ASSERT_EQ(itens.size(), 1);
    EXPECT_EQ(itens.at(0).toObject().value(QStringLiteral("nome")).toString(),
              QStringLiteral("blender"));
    EXPECT_EQ(itens.at(0).toObject().value(QStringLiteral("categoria")).toString(),
              QStringLiteral("gpu"));
}

TEST(CliVramFormatTest, ModoTabularContemNomeECategoria) {
    OpcoesCliVram opts;
    const auto saida = formatar_relatorio(fazer_relatorio(), opts);
    EXPECT_TRUE(saida.contains(QStringLiteral("blender")));
    EXPECT_TRUE(saida.contains(QStringLiteral("gpu")));
    EXPECT_TRUE(saida.contains(QStringLiteral("RTX 4070")));
}

TEST(CliVramFormatTest, RelatorioVazioImprimeMensagemAmigavel) {
    OpcoesCliVram opts;
    const auto saida = formatar_relatorio(RelatorioAuditoriaVram{}, opts);
    EXPECT_TRUE(saida.contains(QStringLiteral("Nenhuma GPU")));
}

TEST(CliVramExecutarTest, ExecutarComServicoUsaMockEretornaZero) {
    auto backend = std::make_shared<MockGpuBackend>();
    auto classificador = std::make_shared<MockClassificadorProcessos>();
    auto locator = std::make_shared<MockConfigLocator>();

    GpuInfo gpu{0, Fabricante::Nvidia, QStringLiteral("Stub"),
                BytesDeMemoria::de_megabytes(8192)};
    Processo proc{PID{42}, NomeProcesso{QStringLiteral("dummy")},
                  BytesDeMemoria::de_megabytes(128), 0,
                  {ChaveProcesso::de_binario(QStringLiteral("dummy"))}};

    EXPECT_CALL(*backend, enumerar_gpus()).WillOnce(Return(std::vector<GpuInfo>{gpu}));
    EXPECT_CALL(*backend, enumerar_processos_vram(0))
        .WillOnce(Return(std::vector<Processo>{proc}));
    EXPECT_CALL(*classificador, classificar(::testing::_))
        .WillOnce(Return(ResultadoClassificacao{
            .categoria = Categoria::Indeterminado,
            .justificativa = Justificativa{.regra = QStringLiteral("none"), .detalhe = {}},
        }));
    EXPECT_CALL(*locator, localizar(::testing::_))
        .WillOnce(Return(QVector<OrigemConfig>{}));

    auto svc_class = std::make_shared<ServicoClassificarProcesso>(classificador);
    auto svc_sugerir = std::make_shared<ServicoSugerirAcao>();
    auto svc_loc = std::make_shared<ServicoLocalizarConfig>(locator);
    auto svc = std::make_shared<ServicoAuditarVramAtual>(backend, svc_class, svc_sugerir, svc_loc);

    QString buf;
    QTextStream out{&buf};
    const int rc = executar_com_servico(svc, {QStringLiteral("--json")}, out);
    EXPECT_EQ(rc, 0);
    EXPECT_TRUE(buf.contains(QStringLiteral("dummy")));
}
