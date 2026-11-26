#include "application/vram/auditoria/relatorio_auditoria_vram.h"
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
#include "application/vram/feedback/servico_registrar_feedback.h"
#include "mocks/mock_repositorio_feedback.h"
#include "presentation/viewmodels/vram_audit_viewmodel.h"

#include <QSignalSpy>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using vramopt::application::GrupoAuditoriaGpu;
using vramopt::application::ItemAuditoriaVram;
using vramopt::application::RelatorioAuditoriaVram;
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
using vramopt::presentation::viewmodels::VramAuditViewModel;
using vramopt::application::ServicoRegistrarFeedback;
using vramopt::domain::EntradaFeedback;
using vramopt::tests::MockRepositorioFeedback;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Field;
using ::testing::Return;

namespace {

RelatorioAuditoriaVram fazerRelatorioExemplo() {
    RelatorioAuditoriaVram r;
    GrupoAuditoriaGpu grupo{
        .gpu = GpuInfo{0, Fabricante::Nvidia, QStringLiteral("RTX 4070"),
                       BytesDeMemoria::de_megabytes(12288)},
        .itens = {},
    };
    ItemAuditoriaVram item{
        .processo = Processo{
            PID{1234},
            NomeProcesso{QStringLiteral("chrome")},
            BytesDeMemoria::de_megabytes(450),
            0,
            {ChaveProcesso::de_binario(QStringLiteral("chrome"))}
        },
        .classificacao = ResultadoClassificacao{
            .categoria = Categoria::CandidatoObvioCpu,
            .justificativa = Justificativa{
                .regra = QStringLiteral("catalogo"),
                .detalhe = QStringLiteral("casou binario=chrome"),
            },
        },
        .sugestao = SugestaoAcao{
            .tipo = TipoSugestao::AjustarConfigDoApp,
            .descricao = QStringLiteral("desligar aceleração"),
        },
        .origens_config = {
            OrigemConfig{
                .categoria = CategoriaOrigem::BrowserChromium,
                .tipo = QStringLiteral("browser-flag"),
                .caminho = QStringLiteral("/home/user/.config/google-chrome/Local State"),
                .chave = QStringLiteral("hardware_acceleration_mode.enabled"),
                .valor_atual = QStringLiteral("true"),
                .valor_sugerido = QStringLiteral("false"),
                .instrucao_humana = QStringLiteral("Em chrome://settings..."),
                .confianca = Confianca::Alta,
                .requer_reinicio_app = true,
                .aplicavel_automatico = false,
            },
        },
    };
    grupo.itens.push_back(std::move(item));
    r.grupos.push_back(std::move(grupo));
    return r;
}

}

TEST(VramAuditViewModelTest, AplicarRelatorioConverteEmissaoQVariant) {
    VramAuditViewModel vm{nullptr};
    QSignalSpy spy{&vm, &VramAuditViewModel::gruposChanged};

    vm.aplicarRelatorio(fazerRelatorioExemplo());

    ASSERT_EQ(spy.count(), 1);
    const auto grupos = vm.grupos();
    ASSERT_EQ(grupos.size(), 1);

    const auto grupo = grupos.first().toMap();
    EXPECT_EQ(grupo.value(QStringLiteral("gpuId")).toInt(), 0);
    EXPECT_EQ(grupo.value(QStringLiteral("gpuFabricante")).toString(),
              QStringLiteral("Nvidia"));
    EXPECT_EQ(grupo.value(QStringLiteral("gpuModelo")).toString(),
              QStringLiteral("RTX 4070"));

    const auto itens = grupo.value(QStringLiteral("itens")).toList();
    ASSERT_EQ(itens.size(), 1);
    const auto item = itens.first().toMap();
    EXPECT_EQ(item.value(QStringLiteral("pid")).toInt(), 1234);
    EXPECT_EQ(item.value(QStringLiteral("nome")).toString(), QStringLiteral("chrome"));
    EXPECT_EQ(item.value(QStringLiteral("categoria")).toString(),
              QStringLiteral("CandidatoObvioCpu"));
    EXPECT_EQ(item.value(QStringLiteral("regra")).toString(),
              QStringLiteral("catalogo"));
    EXPECT_EQ(item.value(QStringLiteral("sugestaoTipo")).toString(),
              QStringLiteral("AjustarConfigDoApp"));

    const auto origens = item.value(QStringLiteral("origensConfig")).toList();
    ASSERT_EQ(origens.size(), 1);
    const auto origem = origens.first().toMap();
    EXPECT_EQ(origem.value(QStringLiteral("categoria")).toString(),
              QStringLiteral("BrowserChromium"));
    EXPECT_EQ(origem.value(QStringLiteral("chave")).toString(),
              QStringLiteral("hardware_acceleration_mode.enabled"));
    EXPECT_EQ(origem.value(QStringLiteral("confianca")).toString(),
              QStringLiteral("Alta"));
    EXPECT_TRUE(origem.value(QStringLiteral("requerReinicio")).toBool());
}

TEST(VramAuditViewModelTest, RelatorioVazioGeraGruposVazios) {
    VramAuditViewModel vm{nullptr};
    vm.aplicarRelatorio(RelatorioAuditoriaVram{});
    EXPECT_TRUE(vm.grupos().isEmpty());
}

TEST(VramAuditViewModelTest, ItemExpoeChaveBinarioParaFeedback) {
    VramAuditViewModel vm{nullptr};
    vm.aplicarRelatorio(fazerRelatorioExemplo());
    const auto item = vm.grupos().first().toMap()
        .value(QStringLiteral("itens")).toList().first().toMap();
    EXPECT_EQ(item.value(QStringLiteral("chaveBinario")).toString(), QStringLiteral("chrome"));
}

TEST(VramAuditViewModelTest, RegistrarFeedbackGravaPelaChaveBinarioEAvisa) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    const auto chave = ChaveProcesso::de_binario(QStringLiteral("firefox"));
    EXPECT_CALL(*repo, buscar(chave)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*repo, inserir(AllOf(Field(&EntradaFeedback::chave, chave),
                                     Field(&EntradaFeedback::veredito, Categoria::PrecisaGpu))));
    VramAuditViewModel vm{nullptr, std::make_shared<ServicoRegistrarFeedback>(repo)};
    QSignalSpy spy{&vm, &VramAuditViewModel::feedbackRegistrado};

    EXPECT_TRUE(vm.feedbackDisponivel());
    vm.registrarFeedback(QStringLiteral("firefox"), QStringLiteral("PrecisaGpu"), QStringLiteral("WebGL"));
    ASSERT_TRUE(spy.wait(2000));
    EXPECT_TRUE(spy.first().at(0).toBool());
}

TEST(VramAuditViewModelTest, RegistrarFeedbackRecusaEntradaInvalidaDoQml) {
    auto repo = std::make_shared<MockRepositorioFeedback>();
    EXPECT_CALL(*repo, inserir(_)).Times(0);
    VramAuditViewModel vm{nullptr, std::make_shared<ServicoRegistrarFeedback>(repo)};
    QSignalSpy spy{&vm, &VramAuditViewModel::feedbackRegistrado};

    vm.registrarFeedback(QStringLiteral("firefox"), QStringLiteral("Talvez"), QString{});
    vm.registrarFeedback(QString{}, QStringLiteral("PrecisaGpu"), QString{});
    vm.registrarFeedback(QString(300, QLatin1Char('x')), QStringLiteral("PrecisaGpu"), QString{});
    ASSERT_EQ(spy.count(), 3);
    for (const auto& args : spy) {
        EXPECT_FALSE(args.at(0).toBool());
    }
}

TEST(VramAuditViewModelTest, SemServicoDeFeedbackAcaoFicaIndisponivel) {
    VramAuditViewModel vm{nullptr};
    QSignalSpy spy{&vm, &VramAuditViewModel::feedbackRegistrado};
    EXPECT_FALSE(vm.feedbackDisponivel());
    vm.registrarFeedback(QStringLiteral("firefox"), QStringLiteral("PrecisaGpu"), QString{});
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.first().at(0).toBool());
}
