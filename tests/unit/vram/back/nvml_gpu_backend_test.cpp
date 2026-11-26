#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "infrastructure/collectors/vram/nvml_adapter.h"
#include "infrastructure/collectors/vram/nvml_gpu_backend.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

using vramopt::domain::Fabricante;
using vramopt::infrastructure::INvmlAdapter;
using vramopt::infrastructure::NvmlDeviceInfo;
using vramopt::infrastructure::NvmlGpuBackend;
using vramopt::infrastructure::NvmlProcessoInfo;

namespace {

class FakeNvmlAdapter : public INvmlAdapter {
public:
    bool                                                 disponivel = true;
    std::vector<NvmlDeviceInfo>                          devices;
    std::vector<std::vector<NvmlProcessoInfo>>           procs_por_idx;
    std::vector<std::vector<NvmlProcessoInfo>>           graphics_por_idx;

    [[nodiscard]] bool isAvailable() const override { return disponivel; }
    [[nodiscard]] std::uint32_t deviceCount() override {
        return static_cast<std::uint32_t>(devices.size());
    }
    [[nodiscard]] std::optional<NvmlDeviceInfo> deviceInfo(std::uint32_t idx) override {
        if (idx >= devices.size()) {
            return std::nullopt;
        }
        return devices[idx];
    }
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processosCompute(std::uint32_t idx) override {
        if (idx >= procs_por_idx.size()) {
            return {};
        }
        return procs_por_idx[idx];
    }
    [[nodiscard]] std::vector<NvmlProcessoInfo>
        processosGraphics(std::uint32_t idx) override {
        if (idx >= graphics_por_idx.size()) {
            return {};
        }
        return graphics_por_idx[idx];
    }
};

void escreverComm(const QString& proc_root, std::uint32_t pid, const QString& comm) {
    const QString dir = QStringLiteral("%1/%2").arg(proc_root).arg(pid);
    QDir{}.mkpath(dir);
    QFile f{dir + QStringLiteral("/comm")};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    f.write((comm + QStringLiteral("\n")).toUtf8());
}

}

TEST(NvmlGpuBackendTest, AdapterIndisponivelDevolveListasVazias) {
    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->disponivel = false;

    NvmlGpuBackend backend{adapter};
    EXPECT_TRUE(backend.enumerar_gpus().empty());
    EXPECT_TRUE(backend.enumerar_processos_vram(0).empty());
}

TEST(NvmlGpuBackendTest, EnumeraGpusComNomeEvram) {
    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {
        NvmlDeviceInfo{.nome = QStringLiteral("NVIDIA RTX 4070"),
                       .vram_total_bytes = 12ULL * 1024 * 1024 * 1024},
        NvmlDeviceInfo{.nome = QStringLiteral("NVIDIA T1000"),
                       .vram_total_bytes = 4ULL * 1024 * 1024 * 1024},
    };

    NvmlGpuBackend backend{adapter};
    const auto gpus = backend.enumerar_gpus();
    ASSERT_EQ(gpus.size(), 2U);
    EXPECT_EQ(gpus[0].id(), 0);
    EXPECT_EQ(gpus[0].fabricante(), Fabricante::Nvidia);
    EXPECT_EQ(gpus[0].modelo(), QStringLiteral("NVIDIA RTX 4070"));
    EXPECT_EQ(gpus[0].vram_total().valor(), 12ULL * 1024 * 1024 * 1024);
    EXPECT_EQ(gpus[1].id(), 1);
}

TEST(NvmlGpuBackendTest, MapeiaProcessosUsandoCommDoProcRoot) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString proc = tmp.path() + QStringLiteral("/proc");
    QDir{}.mkpath(proc);
    escreverComm(proc, 1234, QStringLiteral("blender"));
    escreverComm(proc, 5678, QStringLiteral("ollama"));

    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {
        NvmlDeviceInfo{.nome = QStringLiteral("NVIDIA"),
                       .vram_total_bytes = 8ULL * 1024 * 1024 * 1024},
    };
    adapter->procs_por_idx = {
        {
            NvmlProcessoInfo{.pid = 1234, .bytes = 256ULL * 1024 * 1024},
            NvmlProcessoInfo{.pid = 5678, .bytes = 1024ULL * 1024 * 1024},
        },
    };

    NvmlGpuBackend backend{adapter, proc};
    const auto procs = backend.enumerar_processos_vram(0);
    ASSERT_EQ(procs.size(), 2U);
    EXPECT_EQ(procs[0].pid().valor(), 1234);
    EXPECT_EQ(procs[0].nome().valor(), QStringLiteral("blender"));
    EXPECT_EQ(procs[0].vram_consumida().valor(), 256ULL * 1024 * 1024);
    EXPECT_EQ(procs[0].gpu_id(), 0);
    ASSERT_EQ(procs[0].chaves().size(), 1U);
    EXPECT_EQ(procs[0].chaves().front().valor(), QStringLiteral("blender"));

    EXPECT_EQ(procs[1].pid().valor(), 5678);
    EXPECT_EQ(procs[1].nome().valor(), QStringLiteral("ollama"));
}

TEST(NvmlGpuBackendTest, FallbackParaInterrogacaoQuandoCommAusente) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString proc = tmp.path() + QStringLiteral("/proc");
    QDir{}.mkpath(proc);

    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {
        NvmlDeviceInfo{.nome = QStringLiteral("NVIDIA"),
                       .vram_total_bytes = 0},
    };
    adapter->procs_por_idx = {
        {NvmlProcessoInfo{.pid = 999, .bytes = 1024}},
    };

    NvmlGpuBackend backend{adapter, proc};
    const auto procs = backend.enumerar_processos_vram(0);
    ASSERT_EQ(procs.size(), 1U);
    EXPECT_EQ(procs[0].nome().valor(), QStringLiteral("?"));
}

TEST(NvmlGpuBackendTest, GpuIdNegativoRetornaVazio) {
    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {
        NvmlDeviceInfo{.nome = QStringLiteral("NVIDIA"),
                       .vram_total_bytes = 0},
    };
    NvmlGpuBackend backend{adapter};
    EXPECT_TRUE(backend.enumerar_processos_vram(-1).empty());
}

// Browsers, compositores e jogos usam contexto gráfico, não compute: sem a lista graphics
// a auditoria não via justamente os processos que os localizadores de config tratam.
TEST(NvmlGpuBackendTest, IncluiProcessosGraphicsAlemDosCompute) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    escreverComm(tmp.path(), 100, QStringLiteral("python"));
    escreverComm(tmp.path(), 200, QStringLiteral("firefox"));

    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {NvmlDeviceInfo{.nome = QStringLiteral("RTX"), .vram_total_bytes = 1ULL << 33}};
    adapter->procs_por_idx = {{NvmlProcessoInfo{.pid = 100, .bytes = 1ULL << 30}}};
    adapter->graphics_por_idx = {{NvmlProcessoInfo{.pid = 200, .bytes = 300ULL << 20}}};

    NvmlGpuBackend backend{adapter, tmp.path()};
    const auto procs = backend.enumerar_processos_vram(0);
    ASSERT_EQ(procs.size(), 2U);
    bool viu_firefox = false;
    for (const auto& p : procs) {
        if (p.pid().valor() == 200) {
            viu_firefox = true;
            EXPECT_EQ(p.nome().valor(), QStringLiteral("firefox"));
            EXPECT_EQ(p.vram_consumida().valor(), 300ULL << 20);
        }
    }
    EXPECT_TRUE(viu_firefox);
}

// Processo com contexto compute E graphics aparece nas duas listas: conta uma vez só.
TEST(NvmlGpuBackendTest, DeduplicaPidPresenteNasDuasListas) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    escreverComm(tmp.path(), 300, QStringLiteral("blender"));

    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {NvmlDeviceInfo{.nome = QStringLiteral("RTX"), .vram_total_bytes = 1ULL << 33}};
    adapter->procs_por_idx = {{NvmlProcessoInfo{.pid = 300, .bytes = 500ULL << 20}}};
    adapter->graphics_por_idx = {{NvmlProcessoInfo{.pid = 300, .bytes = 800ULL << 20}}};

    NvmlGpuBackend backend{adapter, tmp.path()};
    const auto procs = backend.enumerar_processos_vram(0);
    ASSERT_EQ(procs.size(), 1U);
    EXPECT_EQ(procs[0].vram_consumida().valor(), 800ULL << 20); // maior dos dois relatos
}

// NVML devolve NVML_VALUE_NOT_AVAILABLE (~0ULL) quando não consegue medir (ex.: sem
// permissão / MIG): não pode virar 16 EiB de VRAM no relatório.
TEST(NvmlGpuBackendTest, ValorNaoDisponivelViraZero) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    escreverComm(tmp.path(), 400, QStringLiteral("Xwayland"));

    auto adapter = std::make_shared<FakeNvmlAdapter>();
    adapter->devices = {NvmlDeviceInfo{.nome = QStringLiteral("RTX"), .vram_total_bytes = 1ULL << 33}};
    adapter->graphics_por_idx = {{NvmlProcessoInfo{.pid = 400, .bytes = ~0ULL}}};

    NvmlGpuBackend backend{adapter, tmp.path()};
    const auto procs = backend.enumerar_processos_vram(0);
    ASSERT_EQ(procs.size(), 1U);
    EXPECT_EQ(procs[0].vram_consumida().valor(), 0U);
}
