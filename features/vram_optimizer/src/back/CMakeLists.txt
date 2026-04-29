qt_add_library(vram_back STATIC
    valueobjects/pid.cpp
    valueobjects/bytes_de_memoria.cpp
    valueobjects/nome_processo.cpp
    valueobjects/chave_processo.cpp
    classificacao/justificativa.cpp
    entidades/processo.cpp
    entidades/gpu_info.cpp
    amostra/amostra_janela.cpp
    amostra/limiares_comportamento.cpp
    entidades/entrada_catalogo.cpp
    entidades/entrada_feedback.cpp
    configuracao/origem_config.cpp
)
target_include_directories(vram_back PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(vram_back PUBLIC Qt6::Core)
vram_aplicar_flags_estritas(vram_back)
