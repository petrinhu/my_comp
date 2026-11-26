#pragma once

#include "domain/vram/interfaces/i_leitor_maps_processo.h"

#include <QString>

namespace my_comp::infrastructure::vram {

// Lê /proc/<pid>/maps e devolve os caminhos únicos de bibliotecas compartilhadas (.so).
class LeitorMapsProc final : public domain::vram::ILeitorMapsProcesso {
public:
    explicit LeitorMapsProc(QString proc_root = QStringLiteral("/proc"));

    [[nodiscard]] QStringList bibliotecas_mapeadas(const domain::vram::PID& pid) const override;

private:
    QString m_proc_root;
};

}
