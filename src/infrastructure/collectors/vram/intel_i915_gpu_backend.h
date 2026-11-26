#pragma once

#include "domain/vram/interfaces/i_gpu_backend.h"

#include <QString>

namespace my_comp::infrastructure::vram {

class IntelI915GpuBackend final : public domain::vram::IGpuBackend {
public:
    explicit IntelI915GpuBackend(QString drm_root = QStringLiteral("/sys/class/drm"),
                                 QString proc_root = QStringLiteral("/proc"));

    [[nodiscard]] std::vector<domain::vram::GpuInfo> enumerar_gpus() const override;

    [[nodiscard]] std::vector<domain::vram::Processo>
        enumerar_processos_vram(int gpu_id) const override;

    [[nodiscard]] static bool sistema_tem_gpu_intel(const QString& drm_root);

private:
    struct CardIntel {
        int                      id_local;
        QString                  card_dir;
        QString                  pci_slot;
        QString                  modelo;
        domain::vram::BytesDeMemoria   vram_total;
    };

    [[nodiscard]] std::vector<CardIntel> descobrir_cards() const;

    QString m_drm_root;
    QString m_proc_root;
};

}
