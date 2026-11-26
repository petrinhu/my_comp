#include "infrastructure/collectors/vram/nvml_gpu_backend.h"

#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <QFile>
#include <QString>

#include <map>
#include <utility>

namespace my_comp::infrastructure::vram {

namespace {

QString ler_comm(const QString& proc_root, std::uint32_t pid) {
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

std::vector<my_comp::domain::vram::GpuInfo>
NvmlGpuBackend::enumerar_gpus() const {
    std::vector<domain::vram::GpuInfo> gpus;
    if (!m_adapter || !m_adapter->is_available()) {
        return gpus;
    }
    const auto count = m_adapter->device_count();
    gpus.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto info = m_adapter->device_info(i);
        if (!info.has_value()) {
            continue;
        }
        gpus.emplace_back(static_cast<int>(i),
                          domain::vram::Fabricante::Nvidia,
                          info->nome,
                          domain::vram::BytesDeMemoria{info->vram_total_bytes});
    }
    return gpus;
}

std::vector<my_comp::domain::vram::Processo>
NvmlGpuBackend::enumerar_processos_vram(int gpu_id) const {
    std::vector<domain::vram::Processo> processos;
    if (!m_adapter || !m_adapter->is_available() || gpu_id < 0) {
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
    acumular(m_adapter->processos_compute(idx));
    acumular(m_adapter->processos_graphics(idx));

    processos.reserve(por_pid.size());
    for (const auto& [pid, bytes] : por_pid) {
        const NvmlProcessoInfo r{.pid = pid, .bytes = bytes};
        const auto nome = ler_comm(m_proc_root, r.pid);
        std::vector<domain::vram::ChaveProcesso> chaves;
        chaves.push_back(domain::vram::ChaveProcesso::de_binario(nome));
        processos.emplace_back(domain::vram::PID{static_cast<std::int32_t>(r.pid)},
                               domain::vram::NomeProcesso{nome},
                               domain::vram::BytesDeMemoria{r.bytes},
                               gpu_id,
                               std::move(chaves));
    }
    return processos;
}

}
