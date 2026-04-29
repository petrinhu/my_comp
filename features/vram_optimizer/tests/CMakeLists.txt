include(GoogleTest)

add_executable(vram_tests
    sanity_test.cpp
    back/pid_test.cpp
    back/bytes_de_memoria_test.cpp
    back/nome_processo_test.cpp
    back/chave_processo_test.cpp
    back/justificativa_test.cpp
    back/resultado_classificacao_test.cpp
    back/processo_test.cpp
    back/gpu_info_test.cpp
    back/amostra_janela_test.cpp
    back/limiares_comportamento_test.cpp
    back/entrada_catalogo_test.cpp
    back/entrada_feedback_test.cpp
    back/interfaces_compilam_test.cpp
    back/origem_config_test.cpp
    mid/regra_feedback_test.cpp
    mid/regra_catalogo_test.cpp
    mid/mapa_categorias_desktop_test.cpp
    mid/regra_categoria_desktop_test.cpp
    mid/lista_libs_compute_test.cpp
    mid/regra_biblioteca_compute_test.cpp
    mid/regra_comportamento_test.cpp
    mid/classificador_em_cascata_test.cpp
    mid/servico_classificar_processo_test.cpp
    mid/integracao_cascata_test.cpp
    mid/servico_sugerir_acao_test.cpp
    mid/localizador_browser_chromium_test.cpp
    mid/localizador_firefox_test.cpp
    mid/localizador_env_var_test.cpp
    mid/localizador_desktop_wrapper_test.cpp
    mid/localizador_composite_test.cpp
    mid/servico_localizar_config_test.cpp
    mid/integracao_localizador_composite_test.cpp
)
target_link_libraries(vram_tests PRIVATE
    vram_back
    vram_mid
    GTest::gtest_main
    GTest::gmock
)
target_include_directories(vram_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
vram_aplicar_flags_estritas(vram_tests)

gtest_discover_tests(vram_tests)
