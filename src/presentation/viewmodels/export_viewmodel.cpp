#include "export_viewmodel.h"
#include <QtConcurrent>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>
#include <QStandardPaths>
#include <QUrl>

namespace my_comp::presentation::viewmodels {

ExportViewModel::ExportViewModel(QObject* parent) : QObject(parent) {}

void ExportViewModel::add_provider(std::unique_ptr<domain::interfaces::IExportProvider> provider) {
    m_providers.push_back(std::move(provider));
}

void ExportViewModel::set_vms(SystemInfoViewModel* system, CpuViewModel* cpu, MemoryViewModel* mem,
                             UserMetadataViewModel* user, HardwareViewModel* hw, DevStackViewModel* dev) {
    m_systemVM = system;
    m_cpuVM = cpu;
    m_memVM = mem;
    m_userVM = user;
    m_hwVM = hw;
    m_devVM = dev;
}

QString ExportViewModel::defaultExportPath() const {
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

domain::models::ReportData ExportViewModel::collect_all_data() {
    domain::models::ReportData data;
    data.title = "Relatório de Sistema - my_comp v2.0";
    data.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (m_systemVM) {
        data.distro = m_systemVM->distro();
        data.kernel = m_systemVM->kernel();
    }

    if (m_userVM) {
        data.user = m_userVM->currentUser();
        QVariantMap userMap;
        userMap.insert("User", m_userVM->currentUser());
        userMap.insert("Shell", m_userVM->shell());
        userMap.insert("Home", m_userVM->home());
        userMap.insert("Grupos", m_userVM->groups());
        data.sections["Usuário e Identidade"] = userMap;
        data.sections["Variáveis de Ambiente"] = m_userVM->envVars();
    }

    if (m_cpuVM) {
        QVariantMap cpuMap;
        cpuMap.insert("Modelo", m_cpuVM->modelName());
        cpuMap.insert("Cores Físicos", m_cpuVM->physicalCores());
        cpuMap.insert("Threads Lógicas", m_cpuVM->logicalThreads());
        cpuMap.insert("Carga Total", QString::number(m_cpuVM->totalLoad(), 'f', 1) + "%");
        data.sections["Processador"] = cpuMap;
    }

    if (m_memVM) {
        QVariantMap memMap;
        memMap.insert("Total", m_memVM->ramTotal());
        memMap.insert("Uso", QString::number(m_memVM->ramUsagePercent(), 'f', 1) + "%");
        memMap.insert("ZRAM Status", m_memVM->hasZram() ? "Ativo" : "Inativo");
        data.sections["Memória RAM"] = memMap;
    }

    if (m_hwVM) {
        data.sections["Hardware (DMI/SMBIOS)"] = m_hwVM->smbios();
        data.sections["Dispositivos USB"] = m_hwVM->usbDevices();
        data.sections["Dispositivos Bluetooth"] = m_hwVM->bluetoothDevices();
    }

    if (m_devVM) {
        data.sections["Ferramentas de Desenvolvimento"] = m_devVM->toolchains();
        data.sections["Containers Ativos"] = m_devVM->containers();
    }

    return data;
}

void ExportViewModel::exportReport(int formatIndex, const QString& filePath) {
    if (formatIndex < 0 || formatIndex >= static_cast<int>(m_providers.size())) {
        emit exportFinished(false, "Formato de exportação inválido.");
        return;
    }

    // Normalizar file:// URIs → caminho local via QUrl (evita parsing ad-hoc de percent-encoding).
    QString cleanPath = filePath;
    QUrl url(filePath);
    if (url.isLocalFile()) cleanPath = url.toLocalFile();

    // Valida permissão de escrita no diretório pai antes de disparar o job assíncrono.
    QFileInfo fi(cleanPath);
    QFileInfo parent(fi.absolutePath());
    if (!parent.exists() || !parent.isDir() || !parent.isWritable()) {
        emit exportFinished(false, "Pasta de destino inválida ou sem permissão de escrita: " + parent.absoluteFilePath());
        return;
    }

    m_isBusy = true;
    emit isBusyChanged();

    QPointer<ExportViewModel> self(this);
    (void)QtConcurrent::run([self, formatIndex, cleanPath]() {
        if (!self) return;
        // collect_all_data acessa cached getters dos VMs (QString/int/QVariantList são
        // copy-on-write com refcount atômico); os VMs rodam updates via QueuedConnection
        // na main thread, leituras concorrentes aqui retornam snapshot válido.
        auto data = self->collect_all_data();
        bool success = self->m_providers[formatIndex]->export_report(data, cleanPath);
        QMetaObject::invokeMethod(self.data(), [self, success, cleanPath]() {
            if (!self) return;
            self->m_isBusy = false;
            emit self->isBusyChanged();
            emit self->exportFinished(success, success ? "Relatório gerado em: " + cleanPath : "Erro ao gerar relatório.");
        }, Qt::QueuedConnection);
    });
}

}
