#pragma once
#include <QObject>
#include <QPointer>
#include <QString>
#include <memory>
#include <vector>
#include "../../domain/interfaces/i_export_provider.h"
#include "../viewmodels/cpu_viewmodel.h"
#include "../viewmodels/memory_viewmodel.h"
#include "../viewmodels/system_info_viewmodel.h"
#include "../viewmodels/user_metadata_viewmodel.h"
#include "../viewmodels/hardware_viewmodel.h"
#include "../viewmodels/dev_stack_viewmodel.h"

namespace my_comp::presentation::viewmodels {

class ExportViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString defaultExportPath READ defaultExportPath CONSTANT)

public:
    explicit ExportViewModel(QObject* parent = nullptr);

    void add_provider(std::unique_ptr<domain::interfaces::IExportProvider> provider);

    // Injeção de dependências para coleta de dados
    void set_vms(SystemInfoViewModel* system, CpuViewModel* cpu, MemoryViewModel* mem,
                 UserMetadataViewModel* user, HardwareViewModel* hw, DevStackViewModel* dev);

    Q_INVOKABLE void exportReport(int formatIndex, const QString& filePath);

    bool isBusy() const { return m_isBusy; }
    QString defaultExportPath() const;

signals:
    void isBusyChanged();
    void exportFinished(bool success, const QString& message);

private:
    std::vector<std::unique_ptr<domain::interfaces::IExportProvider>> m_providers;
    bool m_isBusy = false;

    // Observação defensiva: QPointer zera automaticamente se o QObject for destruído.
    QPointer<SystemInfoViewModel> m_systemVM;
    QPointer<CpuViewModel> m_cpuVM;
    QPointer<MemoryViewModel> m_memVM;
    QPointer<UserMetadataViewModel> m_userVM;
    QPointer<HardwareViewModel> m_hwVM;
    QPointer<DevStackViewModel> m_devVM;

    domain::models::ReportData collect_all_data();
};

}
