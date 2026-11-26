#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include "../../domain/interfaces/i_gpu_provider.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

// Providers concretos (nvidia-smi / sysfs) são escolhidos no composition root (main.cpp, MC-02);
// a coleta roda em QtConcurrent porque nvidia-smi pode levar segundos (MC-05).
class GpuViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList gpus READ gpus NOTIFY gpusChanged)

public:
    explicit GpuViewModel(std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> providers, QObject* parent = nullptr);
    ~GpuViewModel() override = default;

    QVariantList gpus() const { return m_gpus; }

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void update_stats();

signals:
    void gpusChanged();

private:
    void apply_stats(const std::vector<domain::models::GpuStats>& stats);

    // shared_ptr: o worker mantém os providers vivos mesmo se o VM morrer durante a coleta.
    std::vector<std::shared_ptr<domain::interfaces::IGpuProvider>> m_providers;
    QVariantList m_gpus;
    QTimer* m_timer = nullptr;
    bool m_initialized = false;
    bool m_busy = false;
    bool m_has_emitted = false;
};

} // namespace my_comp::presentation::viewmodels
