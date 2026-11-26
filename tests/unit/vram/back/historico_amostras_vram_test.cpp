#include "infrastructure/collectors/vram/gpu_backend_com_historico.h"
#include "infrastructure/collectors/vram/historico_amostras_vram.h"
#include "mocks/mock_gpu_backend.h"

#include "domain/vram/entidades/gpu_info.h"
#include "domain/vram/entidades/processo.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>

using ::testing::Return;

using my_comp::domain::vram::BytesDeMemoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::domain::vram::Fabricante;
using my_comp::domain::vram::GpuInfo;
using my_comp::domain::vram::NomeProcesso;
using my_comp::domain::vram::PID;
using my_comp::domain::vram::Processo;
using my_comp::infrastructure::vram::GpuBackendComHistorico;
using my_comp::infrastructure::vram::HistoricoAmostrasVram;
using my_comp::tests::vram::MockGpuBackend;

using namespace std::chrono_literals;

namespace {

// Relógio manual: os testes avançam o tempo explicitamente (determinístico).
struct RelogioFalso {
    std::chrono::steady_clock::time_point agora{std::chrono::steady_clock::time_point{} + 1000h};
    HistoricoAmostrasVram::Relogio funcao() {
        return [this] { return agora; };
    }
};

Processo proc(int pid, const QString& nome, std::uint64_t mb, int gpu = 0) {
    return Processo{PID{pid}, NomeProcesso{nome}, BytesDeMemoria::de_megabytes(mb), gpu,
                    {ChaveProcesso::de_binario(nome)}};
}

const BytesDeMemoria k_total = BytesDeMemoria::de_megabytes(8192);

}

TEST(HistoricoAmostrasVramTest, SemDadosDevolveZeroAmostras) {
    RelogioFalso relogio;
    const HistoricoAmostrasVram h{relogio.funcao()};
    const auto a = h.amostra_janela(PID{42}, 30s);
    EXPECT_EQ(a.amostras, 0);
    EXPECT_EQ(a.janela, 30s);
    EXPECT_EQ(a.vram_media, BytesDeMemoria{0});
}

TEST(HistoricoAmostrasVramTest, AgregaMediaDaVramNaJanela) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    h.registrar(proc(10, QStringLiteral("foo"), 100), k_total);
    relogio.agora += 10s;
    h.registrar(proc(10, QStringLiteral("foo"), 300), k_total);

    const auto a = h.amostra_janela(PID{10}, 30s);
    EXPECT_EQ(a.amostras, 2);
    EXPECT_EQ(a.vram_media, BytesDeMemoria::de_megabytes(200));
    EXPECT_EQ(a.vram_total_gpu, k_total);
    EXPECT_EQ(a.janela, 30s);
    // Nenhum backend expõe utilização por processo ainda: fica 0 (documentado).
    EXPECT_DOUBLE_EQ(a.compute_utilizacao_media, 0.0);
}

TEST(HistoricoAmostrasVramTest, AmostrasForaDaJanelaNaoEntram) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    h.registrar(proc(10, QStringLiteral("foo"), 1000), k_total);
    relogio.agora += 40s;
    h.registrar(proc(10, QStringLiteral("foo"), 100), k_total);

    const auto a = h.amostra_janela(PID{10}, 30s);
    EXPECT_EQ(a.amostras, 1);
    EXPECT_EQ(a.vram_media, BytesDeMemoria::de_megabytes(100));

    relogio.agora += 31s;
    EXPECT_EQ(h.amostra_janela(PID{10}, 30s).amostras, 0);
}

TEST(HistoricoAmostrasVramTest, PidsSaoIndependentes) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    h.registrar(proc(10, QStringLiteral("foo"), 100), k_total);
    h.registrar(proc(11, QStringLiteral("bar"), 900), k_total);
    EXPECT_EQ(h.amostra_janela(PID{10}, 30s).vram_media, BytesDeMemoria::de_megabytes(100));
    EXPECT_EQ(h.amostra_janela(PID{11}, 30s).vram_media, BytesDeMemoria::de_megabytes(900));
}

TEST(HistoricoAmostrasVramTest, PidReaproveitadoPorOutroProcessoZeraHistorico) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    h.registrar(proc(10, QStringLiteral("antigo"), 4000), k_total);
    relogio.agora += 5s;
    h.registrar(proc(10, QStringLiteral("novo"), 100), k_total);
    const auto a = h.amostra_janela(PID{10}, 30s);
    EXPECT_EQ(a.amostras, 1);
    EXPECT_EQ(a.vram_media, BytesDeMemoria::de_megabytes(100));
}

TEST(HistoricoAmostrasVramTest, LimitaAmostrasPorPid) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    for (std::size_t i = 0; i < HistoricoAmostrasVram::MAXIMO_AMOSTRAS_POR_PID + 50; ++i) {
        h.registrar(proc(10, QStringLiteral("foo"), 100), k_total);
    }
    EXPECT_EQ(h.amostra_janela(PID{10}, 30s).amostras,
              static_cast<int>(HistoricoAmostrasVram::MAXIMO_AMOSTRAS_POR_PID));
}

TEST(HistoricoAmostrasVramTest, DescartaPidsSemAmostraRecente) {
    RelogioFalso relogio;
    HistoricoAmostrasVram h{relogio.funcao()};
    h.registrar(proc(10, QStringLiteral("morto"), 100), k_total);
    relogio.agora += HistoricoAmostrasVram::RETENCAO + 1s;
    h.registrar(proc(11, QStringLiteral("vivo"), 100), k_total);
    EXPECT_EQ(h.pids_rastreados(), 1U);
}

TEST(GpuBackendComHistoricoTest, DelegaERegistraCadaProcessoComTotalDaGpu) {
    RelogioFalso relogio;
    auto historico = std::make_shared<HistoricoAmostrasVram>(relogio.funcao());
    auto interno = std::make_shared<MockGpuBackend>();
    EXPECT_CALL(*interno, enumerar_gpus())
        .WillRepeatedly(Return(std::vector<GpuInfo>{
            GpuInfo{0, Fabricante::Nvidia, QStringLiteral("RTX"), k_total}}));
    EXPECT_CALL(*interno, enumerar_processos_vram(0))
        .WillOnce(Return(std::vector<Processo>{proc(10, QStringLiteral("foo"), 100)}))
        .WillOnce(Return(std::vector<Processo>{proc(10, QStringLiteral("foo"), 300)}));

    const GpuBackendComHistorico backend{interno, historico};
    ASSERT_EQ(backend.enumerar_gpus().size(), 1U);
    const auto primeira = backend.enumerar_processos_vram(0);
    ASSERT_EQ(primeira.size(), 1U);
    EXPECT_EQ(primeira.front().pid(), PID{10});  // repassa o resultado sem alterar
    relogio.agora += 10s;
    (void)backend.enumerar_gpus();
    (void)backend.enumerar_processos_vram(0);

    const auto a = historico->amostra_janela(PID{10}, 30s);
    EXPECT_EQ(a.amostras, 2);
    EXPECT_EQ(a.vram_media, BytesDeMemoria::de_megabytes(200));
    EXPECT_EQ(a.vram_total_gpu, k_total);
}

TEST(GpuBackendComHistoricoTest, GpuDesconhecidaRegistraTotalZero) {
    RelogioFalso relogio;
    auto historico = std::make_shared<HistoricoAmostrasVram>(relogio.funcao());
    auto interno = std::make_shared<MockGpuBackend>();
    EXPECT_CALL(*interno, enumerar_processos_vram(3))
        .WillOnce(Return(std::vector<Processo>{proc(20, QStringLiteral("bar"), 64, 3)}));

    const GpuBackendComHistorico backend{interno, historico};
    (void)backend.enumerar_processos_vram(3);
    const auto a = historico->amostra_janela(PID{20}, 30s);
    EXPECT_EQ(a.amostras, 1);
    EXPECT_EQ(a.vram_total_gpu, BytesDeMemoria{0});
}
