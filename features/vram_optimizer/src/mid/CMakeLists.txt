qt_add_library(vram_mid STATIC
    classificacao/regras/regra_feedback.cpp
    classificacao/regras/regra_catalogo.cpp
    classificacao/regras/mapa_categorias_desktop.cpp
    classificacao/regras/regra_categoria_desktop.cpp
    classificacao/regras/lista_libs_compute.cpp
    classificacao/regras/regra_biblioteca_compute.cpp
    classificacao/regras/regra_comportamento.cpp
    classificacao/classificador_em_cascata.cpp
    classificacao/servico_classificar_processo.cpp
    classificacao/servico_sugerir_acao.cpp
    configuracao/sublocators/tabela_paths_browser_chromium.cpp
    configuracao/sublocators/localizador_browser_chromium.cpp
    configuracao/sublocators/localizador_firefox.cpp
    configuracao/sublocators/localizador_env_var.cpp
    configuracao/sublocators/lista_wrappers_desktop.cpp
    configuracao/sublocators/localizador_desktop_wrapper.cpp
    configuracao/localizador_composite.cpp
    configuracao/servico_localizar_config.cpp
)
target_include_directories(vram_mid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(vram_mid PUBLIC vram_back Qt6::Core)
vram_aplicar_flags_estritas(vram_mid)
