#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStyleHints>
#include <QDebug>
#include <memory>

#include "domain/update_manager.h"
#include "domain/diagnostic_manager.h"
#include "infrastructure/collectors/qprocess_shell_executor.h"
#include "infrastructure/collectors/dnf_update_provider.h"
#include "infrastructure/collectors/flatpak_update_provider.h"
#include "infrastructure/collectors/npm_update_provider.h"
#include "infrastructure/collectors/journal_diagnostic_provider.h"
#include "infrastructure/collectors/linux_cpu_provider.h"
#include "presentation/viewmodels/update_viewmodel.h"
#include "presentation/viewmodels/diagnostic_viewmodel.h"
#include "presentation/viewmodels/cpu_viewmodel.h"
#include "presentation/viewmodels/memory_viewmodel.h"
#include "presentation/viewmodels/firmware_viewmodel.h"
#include "presentation/viewmodels/gpu_viewmodel.h"
#include "presentation/viewmodels/power_viewmodel.h"
#include "presentation/viewmodels/storage_viewmodel.h"
#include "presentation/viewmodels/network_viewmodel.h"
#include "infrastructure/collectors/linux_memory_provider.h"
#include "infrastructure/collectors/dbus_firmware_provider.h"
#include "infrastructure/collectors/linux_storage_provider.h"
#include "infrastructure/collectors/linux_network_provider.h"

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

    // 2. Domínio
    auto update_manager = std::make_shared<my_comp::domain::UpdateManager>();
    auto diagnostic_manager = std::make_shared<my_comp::domain::DiagnosticManager>();
    
    update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::DnfUpdateProvider>(shell));
    update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::FlatpakUpdateProvider>(shell));
    update_manager->register_provider(std::make_unique<my_comp::infrastructure::collectors::NpmUpdateProvider>(shell));

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

    QQmlApplicationEngine engine;
    
    // Injeção de Contexto
    engine.rootContext()->setContextProperty("isSystemDarkInitial", is_system_dark);
    engine.rootContext()->setContextProperty("updateViewModel", &update_vm);
    engine.rootContext()->setContextProperty("diagnosticViewModel", &diagnostic_vm);
    engine.rootContext()->setContextProperty("cpuViewModel", &cpu_vm);
    engine.rootContext()->setContextProperty("memoryViewModel", &memory_vm);
    engine.rootContext()->setContextProperty("firmwareViewModel", &firmware_vm);
    engine.rootContext()->setContextProperty("gpuViewModel", &gpu_vm);
    engine.rootContext()->setContextProperty("powerViewModel", &power_vm);
    engine.rootContext()->setContextProperty("storageViewModel", &storage_vm);
    engine.rootContext()->setContextProperty("networkViewModel", &network_vm);

    using namespace Qt::StringLiterals;
    const QUrl url(u"qrc:/Main.qml"_s);
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
