// AUDIT-33 — libFuzzer harness para parsear_catalogo_json (catálogo de classificação da
// VRAM). O JSON é entrada não confiável (MC-10): o parser não pode travar nem lançar.

#include <cstddef>
#include <cstdint>

#include <QByteArray>

#include "infrastructure/collectors/vram/catalogo_classificacao_json.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 1024 * 1024) {
        return 0;
    }
    const QByteArray json(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    const auto resultado = my_comp::infrastructure::vram::parsear_catalogo_json(json);
    (void)resultado;
    return 0;
}
