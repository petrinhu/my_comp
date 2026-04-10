cmake_minimum_required(VERSION 3.16)

project(my_comp VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Quick 
    QuickControls2 
    WaylandClient 
    Multimedia 
    DBus 
    Network
    Concurrent
)

find_package(OpenSSL REQUIRED)

# Adicionar GoogleTest para TDD/ATDD
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Fontes
set(PROJECT_SOURCES
    src/main.cpp
    src/infrastructure/collectors/linux_cpu_provider.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/collectors/dbus_firmware_provider.cpp
    src/infrastructure/collectors/linux_storage_provider.cpp
    src/infrastructure/collectors/linux_network_provider.cpp
    src/infrastructure/collectors/interactive_process_runner.cpp
    src/infrastructure/collectors/nvidia_gpu_provider.cpp
    src/infrastructure/collectors/sysfs_gpu_provider.cpp
    src/infrastructure/collectors/linux_power_provider.cpp
    src/infrastructure/collectors/linux_system_info_provider.cpp
    src/infrastructure/collectors/installer_adapter.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/infrastructure/collectors/qprocess_shell_executor.cpp
    src/infrastructure/collectors/linux_hardware_provider.cpp
    src/infrastructure/collectors/systemd_service_provider.cpp
    src/infrastructure/collectors/linux_process_provider.cpp
    src/presentation/viewmodels/cpu_viewmodel.cpp
    src/presentation/viewmodels/gpu_viewmodel.cpp
    src/presentation/viewmodels/power_viewmodel.cpp
    src/presentation/viewmodels/system_info_viewmodel.cpp
    src/presentation/viewmodels/installation_viewmodel.cpp
    src/presentation/viewmodels/memory_viewmodel.cpp
    src/presentation/viewmodels/firmware_viewmodel.cpp
    src/presentation/viewmodels/storage_viewmodel.cpp
    src/presentation/viewmodels/network_viewmodel.cpp
    src/presentation/viewmodels/user_metadata_viewmodel.cpp
    src/presentation/viewmodels/hardware_viewmodel.cpp
    src/presentation/viewmodels/service_viewmodel.cpp
    src/presentation/viewmodels/process_viewmodel.cpp
    src/presentation/viewmodels/update_viewmodel.h
    src/presentation/viewmodels/update_package_model.h
    src/presentation/viewmodels/diagnostic_viewmodel.h
    src/presentation/viewmodels/log_entry_model.h
)

# Recurso QML
qt_add_resources(PROJECT_SOURCES
    src/resources.qrc
)

add_executable(my_comp ${PROJECT_SOURCES})

target_link_libraries(my_comp PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Quick 
    Qt6::QuickControls2
    Qt6::WaylandClient
    Qt6::Multimedia
    Qt6::DBus
    Qt6::Network
    Qt6::Concurrent
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Configuração de Testes
enable_testing()

add_executable(unit_tests 
    tests/unit/placeholder_test.cpp
    tests/unit/dnf_update_provider_test.cpp
    tests/unit/flatpak_update_provider_test.cpp
    tests/unit/npm_update_provider_test.cpp
    tests/unit/update_viewmodel_test.cpp
    tests/unit/journal_provider_test.cpp
    tests/unit/memory_provider_test.cpp
    tests/unit/user_metadata_provider_test.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/presentation/viewmodels/update_viewmodel.h
    src/presentation/viewmodels/update_package_model.h
)
target_link_libraries(unit_tests PRIVATE GTest::gtest_main GTest::gmock Qt6::Core Qt6::DBus)
add_test(NAME unit_tests COMMAND unit_tests)

add_executable(acceptance_tests 
    tests/acceptance/placeholder_acceptance.cpp
    tests/acceptance/update_manager_test.cpp
    tests/acceptance/journal_acceptance_test.cpp
    tests/acceptance/journal_hardware_test.cpp
    tests/acceptance/cpu_monitor_test.cpp
    tests/acceptance/memory_monitor_test.cpp
    tests/acceptance/firmware_monitor_test.cpp
    tests/acceptance/storage_monitor_test.cpp
    tests/acceptance/network_monitor_test.cpp
    tests/acceptance/security_vault_test.cpp
    tests/acceptance/gpu_monitor_test.cpp
    tests/acceptance/power_monitor_test.cpp
    tests/acceptance/user_metadata_test.cpp
    tests/acceptance/hardware_topology_test.cpp
    tests/acceptance/systemd_services_test.cpp
    tests/acceptance/process_monitor_test.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/security/openssl_vault.cpp
    src/infrastructure/collectors/dbus_firmware_provider.cpp
    src/infrastructure/collectors/linux_storage_provider.cpp
    src/infrastructure/collectors/linux_network_provider.cpp
    src/infrastructure/collectors/linux_power_provider.cpp
    src/infrastructure/collectors/interactive_process_runner.cpp
    src/infrastructure/collectors/nvidia_gpu_provider.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/infrastructure/collectors/qprocess_shell_executor.cpp
    src/infrastructure/collectors/linux_hardware_provider.cpp
    src/infrastructure/collectors/systemd_service_provider.cpp
    src/infrastructure/collectors/linux_process_provider.cpp
)
target_link_libraries(acceptance_tests PRIVATE GTest::gtest_main GTest::gmock Qt6::Core Qt6::DBus OpenSSL::Crypto)
add_test(NAME acceptance_tests COMMAND acceptance_tests)
