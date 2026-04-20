cmake_minimum_required(VERSION 3.16)

project(my_comp VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 WaylandClient Multimedia DBus Network Concurrent Test)

find_package(OpenSSL REQUIRED)

# Adicionar GoogleTest para TDD/ATDD
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Fontes
set(PROJECT_SOURCES
    src/main.cpp
    src/infrastructure/collectors/qprocess_shell_executor.cpp
    src/infrastructure/collectors/interactive_process_runner.cpp
    src/infrastructure/collectors/installer_adapter.cpp
    src/infrastructure/collectors/linux_cpu_provider.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/collectors/dbus_firmware_provider.cpp
    src/infrastructure/collectors/linux_storage_provider.cpp
    src/infrastructure/collectors/linux_disk_inventory_provider.cpp
    src/infrastructure/collectors/linux_network_provider.cpp
    src/infrastructure/collectors/nvidia_gpu_provider.cpp
    src/infrastructure/collectors/sysfs_gpu_provider.cpp
    src/infrastructure/collectors/linux_power_provider.cpp
    src/infrastructure/collectors/linux_system_info_provider.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/infrastructure/collectors/linux_hardware_provider.cpp
    src/infrastructure/collectors/systemd_service_provider.cpp
    src/infrastructure/collectors/linux_process_provider.cpp
    src/infrastructure/collectors/linux_dev_stack_provider.cpp
    src/infrastructure/collectors/apt_update_provider.cpp
    src/infrastructure/collectors/cargo_update_provider.cpp
    src/infrastructure/collectors/pacman_update_provider.cpp
    src/infrastructure/collectors/snap_update_provider.cpp
    src/infrastructure/audio/qt_audio_provider.cpp
    src/infrastructure/exporters/json_export_provider.cpp
    src/infrastructure/exporters/markdown_export_provider.cpp
    src/infrastructure/exporters/html_export_provider.cpp
    src/infrastructure/security/openssl_vault.cpp
    src/presentation/viewmodels/update_viewmodel.cpp
    src/presentation/viewmodels/update_package_model.cpp
    src/presentation/viewmodels/diagnostic_viewmodel.cpp
    src/presentation/viewmodels/log_entry_model.cpp
    src/presentation/viewmodels/cpu_viewmodel.cpp
    src/presentation/viewmodels/memory_viewmodel.cpp
    src/presentation/viewmodels/firmware_viewmodel.cpp
    src/presentation/viewmodels/gpu_viewmodel.cpp
    src/presentation/viewmodels/power_viewmodel.cpp
    src/presentation/viewmodels/storage_viewmodel.cpp
    src/presentation/viewmodels/network_viewmodel.cpp
    src/presentation/viewmodels/system_info_viewmodel.cpp
    src/presentation/viewmodels/user_metadata_viewmodel.cpp
    src/presentation/viewmodels/hardware_viewmodel.cpp
    src/presentation/viewmodels/service_viewmodel.cpp
    src/presentation/viewmodels/process_viewmodel.cpp
    src/presentation/viewmodels/dev_stack_viewmodel.cpp
    src/presentation/viewmodels/export_viewmodel.cpp
    src/presentation/viewmodels/audio_viewmodel.cpp
    src/presentation/viewmodels/installation_viewmodel.cpp
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

# Unit Tests Target
set(UNIT_TEST_SOURCES
    tests/unit/placeholder_test.cpp
    tests/unit/process_sorting_test.cpp
    tests/unit/dnf_update_provider_test.cpp
    tests/unit/flatpak_update_provider_test.cpp
    tests/unit/npm_update_provider_test.cpp
    tests/unit/update_viewmodel_test.cpp
    tests/unit/journal_provider_test.cpp
    tests/unit/memory_provider_test.cpp
    tests/unit/user_metadata_provider_test.cpp
    tests/unit/diagnostic_viewmodel_test.cpp
    tests/unit/linux_cpu_provider_test.cpp
    tests/unit/linux_system_info_provider_test.cpp
    tests/unit/linux_storage_provider_test.cpp
    tests/unit/linux_network_provider_test.cpp
    tests/unit/linux_power_provider_test.cpp
    tests/unit/linux_hardware_provider_test.cpp
    tests/unit/linux_process_provider_test.cpp
    tests/unit/systemd_service_provider_test.cpp
    tests/unit/linux_dev_stack_provider_test.cpp
    tests/unit/cpu_viewmodel_test.cpp
    tests/unit/openssl_vault_test.cpp
    tests/unit/exporters_test.cpp
    tests/unit/memory_viewmodel_test.cpp
    tests/unit/gpu_viewmodel_test.cpp
    tests/unit/power_viewmodel_test.cpp
    tests/unit/network_viewmodel_test.cpp
    tests/unit/storage_viewmodel_test.cpp
    tests/unit/more_viewmodels_test.cpp
    tests/unit/final_viewmodels_test.cpp
    tests/unit/export_viewmodel_test.cpp
    tests/unit/managers_test.cpp
    tests/unit/apt_update_provider_test.cpp
    tests/unit/cargo_update_provider_test.cpp
    tests/unit/pacman_update_provider_test.cpp
    tests/unit/snap_update_provider_test.cpp
    tests/unit/nvidia_gpu_provider_test.cpp
    tests/unit/sysfs_gpu_provider_test.cpp
    tests/unit/audio_fuzz_test.cpp
    tests/unit/journal_load_test.cpp
    
    src/infrastructure/collectors/qprocess_shell_executor.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/infrastructure/collectors/linux_cpu_provider.cpp
    src/infrastructure/collectors/linux_system_info_provider.cpp
    src/infrastructure/collectors/linux_storage_provider.cpp
    src/infrastructure/collectors/linux_disk_inventory_provider.cpp
    src/infrastructure/collectors/linux_network_provider.cpp
    src/infrastructure/collectors/linux_power_provider.cpp
    src/infrastructure/collectors/linux_hardware_provider.cpp
    src/infrastructure/collectors/linux_process_provider.cpp
    src/infrastructure/collectors/systemd_service_provider.cpp
    src/infrastructure/collectors/linux_dev_stack_provider.cpp
    src/infrastructure/collectors/nvidia_gpu_provider.cpp
    src/infrastructure/collectors/sysfs_gpu_provider.cpp
    src/infrastructure/collectors/apt_update_provider.cpp
    src/infrastructure/collectors/pacman_update_provider.cpp
    src/infrastructure/collectors/snap_update_provider.cpp
    src/infrastructure/collectors/cargo_update_provider.cpp
    src/infrastructure/collectors/dbus_firmware_provider.cpp
    src/infrastructure/collectors/installer_adapter.cpp
    src/infrastructure/collectors/interactive_process_runner.cpp
    src/infrastructure/security/openssl_vault.cpp
    src/infrastructure/exporters/json_export_provider.cpp
    src/infrastructure/exporters/markdown_export_provider.cpp
    src/infrastructure/exporters/html_export_provider.cpp
    src/infrastructure/audio/qt_audio_provider.cpp
    src/presentation/viewmodels/update_viewmodel.cpp
    src/presentation/viewmodels/update_package_model.cpp
    src/presentation/viewmodels/diagnostic_viewmodel.cpp
    src/presentation/viewmodels/log_entry_model.cpp
    src/presentation/viewmodels/cpu_viewmodel.cpp
    src/presentation/viewmodels/memory_viewmodel.cpp
    src/presentation/viewmodels/firmware_viewmodel.cpp
    src/presentation/viewmodels/gpu_viewmodel.cpp
    src/presentation/viewmodels/power_viewmodel.cpp
    src/presentation/viewmodels/network_viewmodel.cpp
    src/presentation/viewmodels/storage_viewmodel.cpp
    src/presentation/viewmodels/system_info_viewmodel.cpp
    src/presentation/viewmodels/user_metadata_viewmodel.cpp
    src/presentation/viewmodels/hardware_viewmodel.cpp
    src/presentation/viewmodels/service_viewmodel.cpp
    src/presentation/viewmodels/process_viewmodel.cpp
    src/presentation/viewmodels/dev_stack_viewmodel.cpp
    src/presentation/viewmodels/export_viewmodel.cpp
    src/presentation/viewmodels/audio_viewmodel.cpp
    src/presentation/viewmodels/installation_viewmodel.cpp
)

add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE GTest::gtest_main GTest::gmock Qt6::Core Qt6::Gui Qt6::DBus Qt6::Network Qt6::Multimedia Qt6::Concurrent Qt6::Test OpenSSL::SSL OpenSSL::Crypto)
add_test(NAME unit_tests COMMAND unit_tests)

# Acceptance Tests Target
set(ACCEPTANCE_TEST_SOURCES
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
    tests/acceptance/dev_stack_test.cpp
    tests/acceptance/export_system_test.cpp
    tests/acceptance/audio_capture_test.cpp
    
    src/infrastructure/collectors/qprocess_shell_executor.cpp
    src/infrastructure/collectors/interactive_process_runner.cpp
    src/infrastructure/collectors/installer_adapter.cpp
    src/infrastructure/collectors/linux_cpu_provider.cpp
    src/infrastructure/collectors/linux_memory_provider.cpp
    src/infrastructure/collectors/dbus_firmware_provider.cpp
    src/infrastructure/collectors/linux_storage_provider.cpp
    src/infrastructure/collectors/linux_disk_inventory_provider.cpp
    src/infrastructure/collectors/linux_network_provider.cpp
    src/infrastructure/collectors/nvidia_gpu_provider.cpp
    src/infrastructure/collectors/sysfs_gpu_provider.cpp
    src/infrastructure/collectors/linux_power_provider.cpp
    src/infrastructure/collectors/linux_system_info_provider.cpp
    src/infrastructure/collectors/linux_user_metadata_provider.cpp
    src/infrastructure/collectors/linux_hardware_provider.cpp
    src/infrastructure/collectors/systemd_service_provider.cpp
    src/infrastructure/collectors/linux_process_provider.cpp
    src/infrastructure/collectors/linux_dev_stack_provider.cpp
    src/infrastructure/collectors/apt_update_provider.cpp
    src/infrastructure/collectors/pacman_update_provider.cpp
    src/infrastructure/collectors/snap_update_provider.cpp
    src/infrastructure/collectors/cargo_update_provider.cpp
    src/infrastructure/audio/qt_audio_provider.cpp
    src/infrastructure/exporters/json_export_provider.cpp
    src/infrastructure/exporters/markdown_export_provider.cpp
    src/infrastructure/exporters/html_export_provider.cpp
    src/infrastructure/security/openssl_vault.cpp
    
    src/presentation/viewmodels/update_viewmodel.cpp
    src/presentation/viewmodels/update_package_model.cpp
    src/presentation/viewmodels/diagnostic_viewmodel.cpp
    src/presentation/viewmodels/log_entry_model.cpp
    src/presentation/viewmodels/cpu_viewmodel.cpp
    src/presentation/viewmodels/memory_viewmodel.cpp
    src/presentation/viewmodels/firmware_viewmodel.cpp
    src/presentation/viewmodels/gpu_viewmodel.cpp
    src/presentation/viewmodels/power_viewmodel.cpp
    src/presentation/viewmodels/storage_viewmodel.cpp
    src/presentation/viewmodels/network_viewmodel.cpp
    src/presentation/viewmodels/system_info_viewmodel.cpp
    src/presentation/viewmodels/user_metadata_viewmodel.cpp
    src/presentation/viewmodels/hardware_viewmodel.cpp
    src/presentation/viewmodels/service_viewmodel.cpp
    src/presentation/viewmodels/process_viewmodel.cpp
    src/presentation/viewmodels/dev_stack_viewmodel.cpp
    src/presentation/viewmodels/export_viewmodel.cpp
    src/presentation/viewmodels/audio_viewmodel.cpp
    src/presentation/viewmodels/installation_viewmodel.cpp
)

add_executable(acceptance_tests ${ACCEPTANCE_TEST_SOURCES})
target_link_libraries(acceptance_tests PRIVATE 
    GTest::gtest_main 
    GTest::gmock 
    Qt6::Core 
    Qt6::DBus 
    Qt6::Concurrent 
    Qt6::Test 
    Qt6::Multimedia 
    Qt6::Network
    OpenSSL::SSL
    OpenSSL::Crypto
)
add_test(NAME acceptance_tests COMMAND acceptance_tests)
