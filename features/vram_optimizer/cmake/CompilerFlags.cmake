# cmake/CompilerFlags.cmake — flags estritas para todos os targets
function(vram_aplicar_flags_estritas alvo)
    target_compile_features(${alvo} PUBLIC cxx_std_23)
    set_target_properties(${alvo} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
    )
    target_compile_options(${alvo} PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast
        -Wnull-dereference
        -fstack-protector-strong
    )
    target_compile_definitions(${alvo} PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:_FORTIFY_SOURCE=3>)
    if(VRAM_HABILITAR_SANITIZERS)
        target_compile_options(${alvo} PRIVATE
            -fsanitize=address,undefined,leak -fno-omit-frame-pointer)
        target_link_options(${alvo} PRIVATE
            -fsanitize=address,undefined,leak)
    endif()
endfunction()
