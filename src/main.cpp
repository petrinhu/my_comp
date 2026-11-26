#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringList>
#include <QStyleHints>
#include <QPalette>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QTimer>
#include <QStandardPaths>
#include <memory>

using namespace Qt::StringLiterals;

#include "application/update_manager.h"
#include "application/diagnostic_manager.h"
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
#include "presentation/viewmodels/audio_viewmodel.h"
#include "infrastructure/audio/qt_audio_provider.h"
#include "infrastructure/exporters/json_export_provider.h"
#include "infrastructure/exporters/markdown_export_provider.h"
#include "infrastructure/exporters/html_export_provider.h"
#include "infrastructure/collectors/linux_cpu_provider.h"
#include "infrastructure/collectors/nvidia_gpu_provider.h"
#include "infrastructure/collectors/sysfs_gpu_provider.h"

#include "infrastructure/collectors/linux_memory_provider.h"
#include "infrastructure/collectors/linux_power_provider.h"
#include "infrastructure/collectors/dbus_firmware_provider.h"
#include "infrastructure/collectors/linux_storage_provider.h"
#include "infrastructure/collectors/linux_disk_inventory_provider.h"
#include "infrastructure/collectors/linux_smart_provider.h"
#include "infrastructure/collectors/hdparm_benchmark_provider.h"
#include "infrastructure/collectors/filesystem_directory_scanner.h"
#include "presentation/viewmodels/directory_scanner_viewmodel.h"
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

// Feature VRAM (auditoria de uso de VRAM por processo)
#include "presentation/cli/cli_vram.h"
#include "composition/vram_composition.h"
#include "presentation/viewmodels/vram_audit_viewmodel.h"

int main(int argc, char *argv[])
{
    // Subcomando CLI: my_comp vram [--json|--all-processes|--no-config-origins]
    if (argc >= 2 && QString::fromUtf8(argv[1]) == QStringLiteral("vram")) {
        QCoreApplication cli_app(argc, argv);
        QStringList sub_args;
        for (int i = 2; i < argc; ++i) {
            sub_args << QString::fromUtf8(argv[i]);
        }
        QTextStream out{stdout};
        return vramopt::presentation::cli::executarComServico(
            vramopt::composition::compor_servico_auditar_vram(), sub_args, out);
    }

    // Wayland por padrão, sem sobrescrever escolha explícita do usuário (ex.: QT_QPA_PLATFORM=xcb).
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "wayland");
    }

    QGuiApplication app(argc, argv);
    app.setOrganizationName("my_comp");
    app.setApplicationName("my_comp");

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const bool is_system_dark = (app.styleHints()->colorScheme() == Qt::ColorScheme::Dark);
#else
    // Qt < 6.5 não expõe colorScheme(): infere pelo contraste da paleta do sistema.
    const QPalette sys_palette = QGuiApplication::palette();
    const bool is_system_dark = sys_palette.color(QPalette::Window).lightness()
                                < sys_palette.color(QPalette::WindowText).lightness();
#endif

    // 1. Infraestrutura
    auto shell = std::make_shared<my_comp::infrastructure::collectors::QProcessShellExecutor>();
    auto cpu_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxCpuProvider>();
    auto memory_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxMemoryProvider>();
    auto firmware_provider = std::make_shared<my_comp::infrastructure::collectors::DBusFirmwareProvider>();
    auto storage_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxStorageProvider>();
    auto disk_inventory_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxDiskInventoryProvider>(shell);
    std::shared_ptr<my_comp::domain::interfaces::ISmartProvider> smart_provider;
    if (QFile::exists("/usr/sbin/smartctl") || QFile::exists("/usr/bin/smartctl")) {
        smart_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxSmartProvider>(shell);
    }
    std::shared_ptr<my_comp::domain::interfaces::IBenchmarkProvider> benchmark_provider;
    if (QFile::exists("/usr/sbin/hdparm") || QFile::exists("/usr/bin/hdparm")) {
        benchmark_provider = std::make_shared<my_comp::infrastructure::collectors::HdparmBenchmarkProvider>(shell);
    }
    auto directory_scanner = std::make_shared<my_comp::infrastructure::collectors::FilesystemDirectoryScanner>();
    auto network_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxNetworkProvider>(shell);
    auto system_info_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxSystemInfoProvider>(shell);
    auto interactive_runner = std::make_shared<my_comp::infrastructure::collectors::InteractiveProcessRunner>();
    auto installer_adapter = std::make_shared<my_comp::infrastructure::collectors::InstallerAdapter>(interactive_runner);
    auto audio_provider = std::make_shared<my_comp::infrastructure::audio::QtAudioProvider>();

    // 2. Domínio
    auto update_manager = std::make_shared<my_comp::application::UpdateManager>();
    auto diagnostic_manager = std::make_shared<my_comp::application::DiagnosticManager>();
    update_manager->set_installer(installer_adapter);

    if (QFile::exists("/usr/bin/dnf")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::DnfUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/apt")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::AptUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/pacman")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::PacmanUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/flatpak")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::FlatpakUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/snap")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::SnapUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/npm")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::NpmUpdateProvider>(shell));
    if (QFile::exists("/usr/bin/cargo")) update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::CargoUpdateProvider>(shell));

    diagnostic_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::JournalDiagnosticProvider>(shell));

    // 3. Apresentação (Heap)
    auto update_vm = std::make_unique<my_comp::presentation::viewmodels::UpdateViewModel>(update_manager);
    auto diagnostic_vm = std::make_unique<my_comp::presentation::viewmodels::DiagnosticViewModel>(diagnostic_manager);
    auto cpu_vm = std::make_unique<my_comp::presentation::viewmodels::CpuViewModel>(cpu_provider);
    auto memory_vm = std::make_unique<my_comp::presentation::viewmodels::MemoryViewModel>(memory_provider);
    auto firmware_vm = std::make_unique<my_comp::presentation::viewmodels::FirmwareViewModel>(firmware_provider);
    // GPU: nvidia-smi quando presente; senão sysfs (genérico AMD/Intel). Só busca no PATH, sem spawn.
    std::vector<std::unique_ptr<my_comp::domain::interfaces::IGpuProvider>> gpu_providers;
    if (!QStandardPaths::findExecutable("nvidia-smi").isEmpty()) {
        gpu_providers.push_back(std::make_unique<my_comp::infrastructure::collectors::NvidiaGpuProvider>(shell));
    } else {
        gpu_providers.push_back(std::make_unique<my_comp::infrastructure::collectors::SysfsGpuProvider>());
    }
    auto gpu_vm = std::make_unique<my_comp::presentation::viewmodels::GpuViewModel>(std::move(gpu_providers));
    auto power_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxPowerProvider>();
    auto power_vm = std::make_unique<my_comp::presentation::viewmodels::PowerViewModel>(power_provider);
    auto storage_vm = std::make_unique<my_comp::presentation::viewmodels::StorageViewModel>(storage_provider, disk_inventory_provider, smart_provider, benchmark_provider);
    auto directory_scanner_vm = std::make_unique<my_comp::presentation::viewmodels::DirectoryScannerViewModel>(directory_scanner);
    auto network_vm = std::make_unique<my_comp::presentation::viewmodels::NetworkViewModel>(network_provider);
    auto system_info_vm = std::make_unique<my_comp::presentation::viewmodels::SystemInfoViewModel>(system_info_provider);
    auto installation_vm = std::make_unique<my_comp::presentation::viewmodels::InstallationViewModel>(installer_adapter);
    auto audio_vm = std::make_unique<my_comp::presentation::viewmodels::AudioViewModel>(audio_provider);
    auto user_metadata_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxUserMetadataProvider>(shell);
    auto user_metadata_vm = std::make_unique<my_comp::presentation::viewmodels::UserMetadataViewModel>(user_metadata_provider);
    auto hardware_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxHardwareProvider>(shell);
    auto hardware_vm = std::make_unique<my_comp::presentation::viewmodels::HardwareViewModel>(hardware_provider);
    auto service_provider = std::make_shared<my_comp::infrastructure::collectors::SystemdServiceProvider>(shell);
    auto service_vm = std::make_unique<my_comp::presentation::viewmodels::ServiceViewModel>(service_provider);
    auto process_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxProcessProvider>();
    auto process_vm = std::make_unique<my_comp::presentation::viewmodels::ProcessViewModel>(process_provider);
    auto dev_stack_provider = std::make_shared<my_comp::infrastructure::collectors::LinuxDevStackProvider>(shell);
    auto dev_stack_vm = std::make_unique<my_comp::presentation::viewmodels::DevStackViewModel>(dev_stack_provider);
    auto export_vm = std::make_unique<my_comp::presentation::viewmodels::ExportViewModel>();
    
    export_vm->add_provider(std::make_unique<my_comp::infrastructure::exporters::JsonExportProvider>());
    export_vm->add_provider(std::make_unique<my_comp::infrastructure::exporters::MarkdownExportProvider>());
    export_vm->add_provider(std::make_unique<my_comp::infrastructure::exporters::HtmlExportProvider>());
    export_vm->set_vms(system_info_vm.get(), cpu_vm.get(), memory_vm.get(), user_metadata_vm.get(), hardware_vm.get(), dev_stack_vm.get());

    // Feature VRAM — composição única compartilhada com a CLI
    auto vram_svc_auditar = vramopt::composition::compor_servico_auditar_vram();

    auto vram_audit_vm = std::make_unique<vramopt::presentation::viewmodels::VramAuditViewModel>(vram_svc_auditar);

    QQmlApplicationEngine engine;
    auto ctx = engine.rootContext();
    ctx->setContextProperty("isSystemDarkInitial", is_system_dark);
    ctx->setContextProperty("updateViewModel", update_vm.get());
    ctx->setContextProperty("diagnosticViewModel", diagnostic_vm.get());
    ctx->setContextProperty("cpuViewModel", cpu_vm.get());
    ctx->setContextProperty("memoryViewModel", memory_vm.get());
    ctx->setContextProperty("firmwareViewModel", firmware_vm.get());
    ctx->setContextProperty("gpuViewModel", gpu_vm.get());
    ctx->setContextProperty("powerViewModel", power_vm.get());
    ctx->setContextProperty("systemInfoViewModel", system_info_vm.get());
    ctx->setContextProperty("userMetadataViewModel", user_metadata_vm.get());
    ctx->setContextProperty("hardwareViewModel", hardware_vm.get());
    ctx->setContextProperty("serviceViewModel", service_vm.get());
    ctx->setContextProperty("processViewModel", process_vm.get());
    ctx->setContextProperty("devStackViewModel", dev_stack_vm.get());
    ctx->setContextProperty("exportViewModel", export_vm.get());
    ctx->setContextProperty("audioViewModel", audio_vm.get());
    ctx->setContextProperty("installationViewModel", installation_vm.get());
    ctx->setContextProperty("storageViewModel", storage_vm.get());
    ctx->setContextProperty("directoryScannerViewModel", directory_scanner_vm.get());
    ctx->setContextProperty("networkViewModel", network_vm.get());
    ctx->setContextProperty("vramAuditViewModel", vram_audit_vm.get());

    const QUrl url(u"qrc:/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                qCritical() << "[Main] Failed to load Main.qml";
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);
    engine.load(url);

    try {
        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "[Main] UNHANDLED EXCEPTION:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "[Main] UNKNOWN ERROR CAUSING CRASH";
        return -1;
    }
}
