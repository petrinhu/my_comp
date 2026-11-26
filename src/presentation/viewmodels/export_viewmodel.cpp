#include "export_viewmodel.h"
#include <QtConcurrent>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>
#include <QVariantMap>

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

QVariantList ExportViewModel::exportHistory() const {
    QSettings settings;
    QVariantList list;
    const int size = settings.beginReadArray(HISTORY_ARRAY_KEY);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QVariantMap entry;
        entry.insert("timestamp", settings.value("timestamp").toString());
        entry.insert("format", settings.value("format").toString());
        entry.insert("path", settings.value("path").toString());
        list.append(entry);
    }
    settings.endArray();
    return list;
}

void ExportViewModel::clearHistory() {
    QSettings settings;
    settings.remove(HISTORY_ARRAY_KEY);
    settings.sync();
    emit exportHistoryChanged();
}

void ExportViewModel::append_to_history(const HistoryEntry& item) {
    QSettings settings;

    QVariantList entries;
    const int size = settings.beginReadArray(HISTORY_ARRAY_KEY);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QVariantMap entry;
        entry.insert("timestamp", settings.value("timestamp").toString());
        entry.insert("format", settings.value("format").toString());
        entry.insert("path", settings.value("path").toString());
        entries.append(entry);
    }
    settings.endArray();

    QVariantMap new_entry;
    new_entry.insert("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    new_entry.insert("format", item.format_name);
    new_entry.insert("path", item.file_path);
    entries.append(new_entry);

    while (entries.size() > MAX_HISTORY_ENTRIES) {
        entries.removeFirst();
    }

    settings.remove(HISTORY_ARRAY_KEY);
    settings.beginWriteArray(HISTORY_ARRAY_KEY);
    for (int i = 0; i < entries.size(); ++i) {
        settings.setArrayIndex(i);
        const QVariantMap entry = entries[i].toMap();
        settings.setValue("timestamp", entry.value("timestamp"));
        settings.setValue("format", entry.value("format"));
        settings.setValue("path", entry.value("path"));
    }
    settings.endArray();
    settings.sync();
}

domain::models::ReportData ExportViewModel::collect_all_data() {
    domain::models::ReportData data;
    data.title = "Relatório de Sistema - my_comp v2.0";
    data.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (auto* system = m_systemVM.data()) {
        data.distro = system->distro();
        data.kernel = system->kernel();
    }

    if (auto* user = m_userVM.data()) {
        data.user = user->currentUser();
        QVariantMap userMap;
        userMap.insert("User", user->currentUser());
        userMap.insert("Shell", user->shell());
        userMap.insert("Home", user->home());
        userMap.insert("Grupos", user->groups());
        data.sections["Usuário e Identidade"] = userMap;
        data.sections["Variáveis de Ambiente"] = user->envVars();
    }

    if (auto* cpu = m_cpuVM.data()) {
        QVariantMap cpuMap;
        cpuMap.insert("Modelo", cpu->modelName());
        cpuMap.insert("Cores Físicos", cpu->physicalCores());
        cpuMap.insert("Threads Lógicas", cpu->logicalThreads());
        cpuMap.insert("Carga Total", QString::number(cpu->totalLoad(), 'f', 1) + "%");
        data.sections["Processador"] = cpuMap;
    }

    if (auto* mem = m_memVM.data()) {
        QVariantMap memMap;
        memMap.insert("Total", mem->ramTotal());
        memMap.insert("Uso", QString::number(mem->ramUsagePercent(), 'f', 1) + "%");
        memMap.insert("ZRAM Status", mem->hasZram() ? "Ativo" : "Inativo");
        data.sections["Memória RAM"] = memMap;
    }

    if (auto* hw = m_hwVM.data()) {
        data.sections["Hardware (DMI/SMBIOS)"] = hw->smbios();
        data.sections["Dispositivos USB"] = hw->usbDevices();
        data.sections["Dispositivos Bluetooth"] = hw->bluetoothDevices();
    }

    if (auto* dev = m_devVM.data()) {
        data.sections["Ferramentas de Desenvolvimento"] = dev->toolchains();
        data.sections["Containers Ativos"] = dev->containers();
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

    // Snapshot na thread principal: os VMs são escritos aqui (QueuedConnection), então lê-los
    // no worker era data race — implicit sharing do Qt é reentrante, não thread-safe para a
    // mesma instância. O worker recebe só cópias por valor (MC-05, AUDITORIAS §I4).
    const domain::models::ReportData data = collect_all_data();
    const auto provider = m_providers[static_cast<std::size_t>(formatIndex)];
    const QString format_name = provider->get_format_name();

    QPointer<ExportViewModel> self(this);
    (void)QtConcurrent::run([self, provider, data, cleanPath, format_name]() {
        const bool success = provider->export_report(data, cleanPath);
        QMetaObject::invokeMethod(self.data(), [self, success, cleanPath, format_name]() {
            auto* vm = self.data();
            if (!vm) return;
            vm->m_isBusy = false;
            emit vm->isBusyChanged();
            if (success) {
                vm->append_to_history({.format_name = format_name, .file_path = cleanPath});
                emit vm->exportHistoryChanged();
            }
            emit vm->exportFinished(success, success ? "Relatório gerado em: " + cleanPath : "Erro ao gerar relatório.");
        }, Qt::QueuedConnection);
    });
}

}
