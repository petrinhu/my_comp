#pragma once
#include <QObject>
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

public:
    explicit ExportViewModel(QObject* parent = nullptr);

    void add_provider(std::unique_ptr<domain::interfaces::IExportProvider> provider);
    
    // Injeção de dependências para coleta de dados
    void set_vms(SystemInfoViewModel* system, CpuViewModel* cpu, MemoryViewModel* mem, 
                 UserMetadataViewModel* user, HardwareViewModel* hw, DevStackViewModel* dev);

    Q_INVOKABLE void exportReport(int formatIndex, const QString& filePath);

    bool isBusy() const { return m_isBusy; }

signals:
    void isBusyChanged();
    void exportFinished(bool success, const QString& message);

private:
    std::vector<std::unique_ptr<domain::interfaces::IExportProvider>> m_providers;
    bool m_isBusy = false;

    // Ponteiros para os outros VMs (não-proprietários)
    SystemInfoViewModel* m_systemVM = nullptr;
    CpuViewModel* m_cpuVM = nullptr;
    MemoryViewModel* m_memVM = nullptr;
    UserMetadataViewModel* m_userVM = nullptr;
    HardwareViewModel* m_hwVM = nullptr;
    DevStackViewModel* m_devVM = nullptr;

    domain::models::ReportData collect_all_data();
};

}
