#include "infrastructure/collectors/vram/nvml_gpu_backend.h"

#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <QFile>
#include <QString>

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
    const auto raw = m_adapter->processosCompute(static_cast<std::uint32_t>(gpu_id));
    processos.reserve(raw.size());
    for (const auto& r : raw) {
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
