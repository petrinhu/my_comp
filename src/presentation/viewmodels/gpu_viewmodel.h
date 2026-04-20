#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include "../../domain/interfaces/i_gpu_provider.h"
#include "../../infrastructure/collectors/i_shell_executor.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

class GpuViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList gpus READ gpus NOTIFY gpusChanged)

public:
    explicit GpuViewModel(std::shared_ptr<infrastructure::collectors::IShellExecutor> executor, QObject* parent = nullptr);
    explicit GpuViewModel(std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> providers, QObject* parent = nullptr);
    ~GpuViewModel() override = default;

    QVariantList gpus() const { return m_gpus; }

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void update_stats();

signals:
    void gpusChanged();

private:
    std::shared_ptr<infrastructure::collectors::IShellExecutor> m_executor;
    std::vector<std::unique_ptr<domain::interfaces::IGpuProvider>> m_providers;
    QVariantList m_gpus;
    QTimer* m_timer;
    bool m_initialized = false;
};

} // namespace my_comp::presentation::viewmodels
