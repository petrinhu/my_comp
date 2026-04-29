# CMakeLists da feature vram_optimizer integrada ao my_comp.
# project()/cmake_minimum_required/enable_testing/GTest vêm do CMakeLists raiz.
# AUTOMOC/AUTORCC/AUTOUIC já estão habilitados globalmente pelo raiz.

option(VRAM_HABILITAR_SANITIZERS "Habilita sanitizers ASan+UBSan+LSan nos targets vram_*" OFF)
option(VRAM_BUILD_TESTS          "Compila testes da feature vram_optimizer"               ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(CompilerFlags)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(src/back)
add_subdirectory(src/mid)

if(VRAM_BUILD_TESTS)
    add_subdirectory(tests)
endif()
