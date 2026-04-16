#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include "../../domain/interfaces/i_gpu_provider.h"
#include "../../infrastructure/collectors/i_shell_executor.h"
#include <memory>
#include <vector>

namespace my_comp::presentation::viewmodels {

using namespace my_comp::domain::interfaces;
using namespace my_comp::infrastructure::collectors;

class GpuViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList gpus READ gpus NOTIFY gpusChanged)

public:
    explicit GpuViewModel(std::shared_ptr<IShellExecutor> executor, QObject* parent = nullptr);
    explicit GpuViewModel(std::vector<std::unique_ptr<IGpuProvider>> providers, QObject* parent = nullptr);
    ~GpuViewModel() override = default;

    QVariantList gpus() const { return m_gpus; }

    Q_INVOKABLE void update_stats();

signals:
    void gpusChanged();

private:
    std::shared_ptr<IShellExecutor> m_executor;
    std::vector<std::unique_ptr<IGpuProvider>> m_providers;
    QVariantList m_gpus;
    QTimer* m_timer;
};

} // namespace my_comp::presentation::viewmodels
