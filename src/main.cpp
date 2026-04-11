#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStyleHints>
#include <QDebug>
#include <QFile>
#include <memory>

#include "domain/update_manager.h"
#include "domain/diagnostic_manager.h"
#include "infrastructure/collectors/qprocess_shell_executor.h"
#include "infrastructure/collectors/dnf_update_provider.h"
#include "infrastructure/collectors/flatpak_update_provider.h"
#include "infrastructure/collectors/npm_update_provider.h"
#include "infrastructure/collectors/snap_update_provider.h"
#include "infrastructure/collectors/cargo_update_provider.h"
#include "infrastructure/collectors/apt_update_provider.h"
#include "infrastructure/collectors/pacman_update_provider.h"
#include "infrastructure/collectors/journal_diagnostic_provider.h"

#include "presentation/viewmodels/update_viewmodel.h"
#include "presentation/viewmodels/diagnostic_viewmodel.h"
#include "presentation/viewmodels/cpu_viewmodel.h"
#include "presentation/viewmodels/memory_viewmodel.h"
#include "presentation/viewmodels/firmware_viewmodel.h"
#include "presentation/viewmodels/gpu_viewmodel.h"
#include "presentation/viewmodels/system_info_viewmodel.h"
#include "presentation/viewmodels/power_viewmodel.h"
#include "presentation/viewmodels/storage_viewmodel.h"
#include "presentation/viewmodels/network_viewmodel.h"
#include "presentation/viewmodels/user_metadata_viewmodel.h"
#include "presentation/viewmodels/hardware_viewmodel.h"
#include "presentation/viewmodels/service_viewmodel.h"
#include "presentation/viewmodels/process_viewmodel.h"
#include "presentation/viewmodels/dev_stack_viewmodel.h"
#include "presentation/viewmodels/export_viewmodel.h"
#include "infrastructure/exporters/json_export_provider.h"
#include "infrastructure/exporters/markdown_export_provider.h"
#include "infrastructure/exporters/html_export_provider.h"
#include "infrastructure/collectors/linux_cpu_provider.h"

#include "infrastructure/collectors/linux_memory_provider.h"
#include "infrastructure/collectors/dbus_firmware_provider.h"
#include "infrastructure/collectors/linux_storage_provider.h"
#include "infrastructure/collectors/linux_network_provider.h"
#include "infrastructure/collectors/linux_system_info_provider.h"
#include "infrastructure/collectors/linux_user_metadata_provider.h"
#include "infrastructure/collectors/linux_hardware_provider.h"
#include "infrastructure/collectors/systemd_service_provider.h"
#include "infrastructure/collectors/linux_process_provider.h"
#include "infrastructure/collectors/linux_dev_stack_provider.h"

#include "infrastructure/collectors/interactive_process_runner.h"
#include "infrastructure/collectors/installer_adapter.h"
#include "presentation/viewmodels/installation_viewmodel.h"

int main(int argc, char *argv[])
{
    // Mandato Wayland Nativo
    qputenv("QT_QPA_PLATFORM", "wayland");

    QGuiApplication app(argc, argv);
    qDebug() << "[Main] my_comp v2.0 Alpha starting...";

    app.setOrganizationName("my_comp");
    app.setApplicationName("my_comp");

    // Detecção de Tema do Sistema
    bool is_system_dark = (app.styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    qDebug() << "[Main] System Color Scheme is" << (is_system_dark ? "Dark" : "Light");

    // 1. Infraestrutura
    auto shell = std::make_shared<my_comp::infrastructure::collectors::QProcessShellExecutor>();
    auto cpu_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxCpuProvider>();
    auto memory_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxMemoryProvider>();
    auto firmware_provider = std::make_shared<my_comp::infrastructure::collectors::DBusFirmwareProvider>();
    auto storage_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxStorageProvider>();
    auto network_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxNetworkProvider>(shell);
    auto system_info_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxSystemInfoProvider>(shell);
    auto interactive_runner = std::make_shared<my_comp::infrastructure::collectors::InteractiveProcessRunner>();
    auto installer_adapter = std::make_shared<my_comp::infrastructure::collectors::InstallerAdapter>(interactive_runner);

    // 2. Domínio
    auto update_manager = std::make_shared<my_comp::domain::UpdateManager>();
    auto diagnostic_manager = std::make_shared<my_comp::domain::DiagnosticManager>();
    
    update_manager->set_installer(installer_adapter);

    // Provedores de Sistema (Detectar Distro)
    if (QFile::exists("/usr/bin/dnf"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::DnfUpdateProvider>(shell));
    else if (QFile::exists("/usr/bin/apt"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::AptUpdateProvider>(shell));
    else if (QFile::exists("/usr/bin/pacman"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::PacmanUpdateProvider>(shell));

    // Provedores Universais/Dev
    if (QFile::exists("/usr/bin/flatpak"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::FlatpakUpdateProvider>(shell));
    
    if (QFile::exists("/usr/bin/snap"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::SnapUpdateProvider>(shell));
    
    if (QFile::exists("/usr/bin/npm"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::NpmUpdateProvider>(shell));
    
    if (QFile::exists("/usr/bin/cargo"))
        update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::CargoUpdateProvider>(shell));

    diagnostic_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::JournalDiagnosticProvider>(shell));

    // 3. Apresentação
    my_comp::presentation::viewmodels::UpdateViewModel update_vm(update_manager);
    my_comp::presentation::viewmodels::DiagnosticViewModel diagnostic_vm(diagnostic_manager);
    my_comp::presentation::viewmodels::CpuViewModel cpu_vm(cpu_provider);
    my_comp::presentation::viewmodels::MemoryViewModel memory_vm(memory_provider);
    my_comp::presentation::viewmodels::FirmwareViewModel firmware_vm(firmware_provider);
    my_comp::presentation::viewmodels::GpuViewModel gpu_vm;
    my_comp::presentation::viewmodels::PowerViewModel power_vm;
    my_comp::presentation::viewmodels::StorageViewModel storage_vm(storage_provider);
    my_comp::presentation::viewmodels::NetworkViewModel network_vm(network_provider);
    my_comp::presentation::viewmodels::SystemInfoViewModel system_info_vm(system_info_provider);
    my_comp::presentation::viewmodels::InstallationViewModel installation_vm(installer_adapter);

    auto user_metadata_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxUserMetadataProvider>(shell);
    my_comp::presentation::viewmodels::UserMetadataViewModel user_metadata_vm(user_metadata_provider);

    auto hardware_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxHardwareProvider>(shell);
    my_comp::presentation::viewmodels::HardwareViewModel hardware_vm(hardware_provider);

    auto service_provider = std::make_shared<my_comp::infrastructure::collectors::SystemdServiceProvider>(shell);
    my_comp::presentation::viewmodels::ServiceViewModel service_vm(service_provider);

    auto process_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxProcessProvider>();
    my_comp::presentation::viewmodels::ProcessViewModel process_vm(process_provider);

    auto dev_stack_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxDevStackProvider>(shell);
    my_comp::presentation::viewmodels::DevStackViewModel dev_stack_vm(dev_stack_provider);

    my_comp::presentation::viewmodels::ExportViewModel export_vm;
    export_vm.add_provider(std::make_unique<my_comp::infrastructure::exporters::JsonExportProvider>());
    export_vm.add_provider(std::make_unique<my_comp::infrastructure::exporters::MarkdownExportProvider>());
    export_vm.add_provider(std::make_unique<my_comp::infrastructure::exporters::HtmlExportProvider>());
    export_vm.set_vms(&system_info_vm, &cpu_vm, &memory_vm, &user_metadata_vm, &hardware_vm, &dev_stack_vm);

    QQmlApplicationEngine engine;
    
    // Injeção de Contexto
    engine.rootContext()->setContextProperty("isSystemDarkInitial", is_system_dark);
    engine.rootContext()->setContextProperty("updateViewModel", &update_vm);
    engine.rootContext()->setContextProperty("diagnosticViewModel", &diagnostic_vm);
    engine.rootContext()->setContextProperty("cpuViewModel", &cpu_vm);
    engine.rootContext()->setContextProperty("memoryViewModel", &memory_vm);
    engine.rootContext()->setContextProperty("firmwareViewModel", &firmware_vm);
    engine.rootContext()->setContextProperty("systemInfoViewModel", &system_info_vm);
    engine.rootContext()->setContextProperty("userMetadataViewModel", &user_metadata_vm);
    engine.rootContext()->setContextProperty("hardwareViewModel", &hardware_vm);
    engine.rootContext()->setContextProperty("serviceViewModel", &service_vm);
    engine.rootContext()->setContextProperty("processViewModel", &process_vm);
    engine.rootContext()->setContextProperty("devStackViewModel", &dev_stack_vm);
    engine.rootContext()->setContextProperty("exportViewModel", &export_vm);
    engine.rootContext()->setContextProperty("installationViewModel", &installation_vm);
    engine.rootContext()->setContextProperty("gpuViewModel", &gpu_vm);
    engine.rootContext()->setContextProperty("powerViewModel", &power_vm);
    engine.rootContext()->setContextProperty("storageViewModel", &storage_vm);
    engine.rootContext()->setContextProperty("networkViewModel", &network_vm);

    const QUrl url(u"qrc:/my_comp/src/presentation/qml/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "[Main] Failed to create QML object!";
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}
