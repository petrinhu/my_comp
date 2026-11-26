#include "infrastructure/collectors/vram/nvml_gpu_backend.h"

#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <QFile>
#include <QString>

#include <map>
#include <utility>

namespace vramopt::infrastructure {

namespace {

QString lerComm(const QString& proc_root, std::uint32_t pid) {
    QFile f{QStringLiteral("%1/%2/comm").arg(proc_root).arg(pid)};
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral("?");
    }
    QString s = QString::fromUtf8(f.readAll()).trimmed();
    if (s.isEmpty()) {
        return QStringLiteral("?");
    }
    return s;
}

}

NvmlGpuBackend::NvmlGpuBackend(std::shared_ptr<INvmlAdapter> adapter,
                               QString proc_root)
    : m_adapter{std::move(adapter)},
      m_proc_root{std::move(proc_root)} {}

std::vector<vramopt::domain::GpuInfo>
NvmlGpuBackend::enumerarGpus() const {
    std::vector<domain::GpuInfo> gpus;
    if (!m_adapter || !m_adapter->isAvailable()) {
        return gpus;
    }
    const auto count = m_adapter->deviceCount();
    gpus.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto info = m_adapter->deviceInfo(i);
        if (!info.has_value()) {
            continue;
        }
        gpus.emplace_back(static_cast<int>(i),
                          domain::Fabricante::Nvidia,
                          info->nome,
                          domain::BytesDeMemoria{info->vram_total_bytes});
    }
    return gpus;
}

std::vector<vramopt::domain::Processo>
NvmlGpuBackend::enumerarProcessosVram(int gpu_id) const {
    std::vector<domain::Processo> processos;
    if (!m_adapter || !m_adapter->isAvailable() || gpu_id < 0) {
        return processos;
    }
    const auto idx = static_cast<std::uint32_t>(gpu_id);
    // Compute + graphics: um PID pode ter os dois contextos; fica o maior relato
    // (o NVML reporta o uso do processo, não por contexto — somar contaria em dobro).
    std::map<std::uint32_t, std::uint64_t> por_pid;
    auto acumular = [&por_pid](const std::vector<NvmlProcessoInfo>& lista) {
        for (const auto& r : lista) {
            const std::uint64_t bytes = (r.bytes == NVML_VALOR_NAO_DISPONIVEL) ? 0U : r.bytes;
            auto [it, inserido] = por_pid.try_emplace(r.pid, bytes);
            if (!inserido && bytes > it->second) {
                it->second = bytes;
            }
        }
    };
    acumular(m_adapter->processosCompute(idx));
    acumular(m_adapter->processosGraphics(idx));

    processos.reserve(por_pid.size());
    for (const auto& [pid, bytes] : por_pid) {
        const NvmlProcessoInfo r{.pid = pid, .bytes = bytes};
        const auto nome = lerComm(m_proc_root, r.pid);
        std::vector<domain::ChaveProcesso> chaves;
        chaves.push_back(domain::ChaveProcesso::deBinario(nome));
        processos.emplace_back(domain::PID{static_cast<std::int32_t>(r.pid)},
                               domain::NomeProcesso{nome},
                               domain::BytesDeMemoria{r.bytes},
                               gpu_id,
                               std::move(chaves));
    }
    return processos;
}

}
